#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <parser.h>
#include <lexer.h>
#include <def.h>
#include <panic.h>
#include <codegen.h>
#include <symbol.h>
#include <ast.h>

#define PARSE_DEBUG 0

static struct token last_token;
static uint8_t is_eof = 0;
static size_t current_func_id = 0;

#define SCAN is_eof = !scan(&last_token)

static inline void passert(TOKEN_TYPE type, const char* what) {
  if (last_token.type != type) {
    printf(PANIC "Expected \"%s\" (line %d).\n", what, last_token.line);
#if PARSE_DEBUG
    printf(PANIC "TOKEN=%d\n", last_token.type);
#endif
    exit(1);
  }
}

/*
 *  Verify that value is in range
 *  of a type.
 */

/*
static inline void assert_type(INTLIT value, SYM_PTYPE ptype) {
  switch (ptype) {
    case P_U8:
      if (value < 0 || value > UCHAR_MAX) {
        printf(PANIC "Value invalid for type 'u8' (line %d).\n", last_token.line);
        panic();
      }
      break;
  }
}
*/


static NODE_TYPE arithop(TOKEN_TYPE tok) {
  switch (tok) {
    case TT_PLUS:
      return A_ADD;
    case TT_MINUS:
      return A_SUB;
    case TT_STAR:
      return A_MUL;
    case TT_SLASH:
      return A_DIV;
    default:
      printf("%d\n", last_token.type);
      printf(PANIC "__INTERNAL_ERROR__: Unrecognized token found in %s()\n", __func__);
      exit(1);
      break;
  }

  return 0;
}

static struct ast_node* primary_factor(void) {
  struct ast_node* n;

  switch (last_token.type) {
    case TT_INTLIT:
      n = mkastleaf(A_INTLIT, last_token.val_int);
      SCAN;
      return n;
    default:
#if PARSE_DEBUG
      printf("Token=%d\n", last_token.type);
#endif
      printf(PANIC "Synatx error (line %d).\n", last_token.line);
      exit(1);
  }

  return NULL;
}


/*
 *  Returns an AST tree that has it's root as a binary
 *  operator.
 *
 *
 */
static struct ast_node* binexpr(void) {
  struct ast_node* n;

  /*
   *  Fetch the left-hand integer 
   *  literal and scan in the next
   *  token as well.
   *
   */

  struct ast_node* left = primary_factor();

  if (is_eof || last_token.type == TT_RPAREN || last_token.type == TT_SEMI)
    return left;

  NODE_TYPE node_type = arithop(last_token.type);
  SCAN;
  
  /*
   *  Get the right-hand tree.
   */
  struct ast_node* right = binexpr();
  n = mkastnode(node_type, left, NULL, right, 0);
  return n;
}


/*
 *  Parse inline assembly.
 *
 *  __asm {
 *    "stuff"
 *  };
 *
 */

static struct ast_node* inline_assembly(void) {
  passert(TT_ASM, "__asm");
  SCAN;

  passert(TT_LBRACE, "{");
  SCAN;

  passert(TT_STR_CONSTANT, "string constant");
  
  struct ast_node* n = mkastleaf(A_INLINE_ASM, 0);
  n->text = strdup(scanner_textbuf);
  scanner_reset_textbuf();
  SCAN;

  passert(TT_RBRACE, "}");
  SCAN;

  passert(TT_SEMI, ";");
  SCAN;
  return n;
}


/*
 *  Parse a return statement.
 *
 */

static struct ast_node* return_statement(void) {
  if (g_symtbl[current_func_id].ptype == P_NONE) {
    printf(PANIC "Cannot use return statement with function that returns none (line %d).\n", last_token.line);
    exit(1);
  }

  passert(TT_RETURN, "return");
  SCAN;

  struct ast_node* expr = binexpr();
  passert(TT_SEMI, ";");
  SCAN;

  return mkastunary(A_RETURN, expr, 0);
}

/*
 *  Parse a compound statement.
 * 
 */

static struct ast_node* compound_statement(void) {
  struct ast_node* tree = NULL;
  struct ast_node* left = NULL;

  passert(TT_LBRACE, "{");
  SCAN;

  while (1) {
    if (last_token.type == TT_RBRACE) {
      SCAN;
      return left;
    }

    switch (last_token.type) {
      case TT_ASM:
        tree = inline_assembly();
        break;
      case TT_RETURN:
        tree = return_statement();
        break;
      default:
        printf(PANIC "Invalid token (line %d)\n", last_token.line);
        exit(1);
        break;
    } 

    if (tree != NULL) {
      if (left == NULL)
        left = tree;
      else
        left = mkastnode(A_GLUE, left, NULL, tree, 0);
    }
  }
  
  __builtin_unreachable();
}

/*
 *  Parse functions.
 *
 *  symbol() -> type
 *  
 */

static struct ast_node* function(void) {
  uint8_t is_global = 0;

  if (last_token.type == TT_GLOBAL) {
    is_global = 1;
    SCAN;
  }

  passert(TT_ID, "identifier");
  SCAN;


  size_t symbol_slot = symtbl_push_glob(scanner_idbuf, S_FUNCTION);
  current_func_id = symbol_slot;
  g_symtbl[symbol_slot].is_global = is_global;

  passert(TT_LPAREN, "(");
  SCAN;

  passert(TT_RPAREN, ")");
  SCAN;

  passert(TT_MINUS, "->");
  SCAN;

  passert(TT_GT, "->");
  SCAN;

  switch (last_token.type) {
    case TT_NONE:
      g_symtbl[symbol_slot].ptype = P_NONE;
      break;
    case TT_U8:
      g_symtbl[symbol_slot].ptype = P_U8;
      break;
    default:
      printf(PANIC "Invalid function type (line %d) %d\n", last_token.line, last_token.type);
      exit(1);
  }

  SCAN;

  struct ast_node* block_statement = compound_statement();
  return mkastunary(A_FUNC, block_statement, symbol_slot);
}

size_t get_cur_function(void) {
  return current_func_id;
}

void parse(void) {
  SCAN;
  codegen_init();
  init_symtbls();
  while (!(is_eof)) {
    struct ast_node* tree = function();

    if (tree != NULL)
      gen_code(tree);
    else
      break;
  }

  codegen_end();
}
