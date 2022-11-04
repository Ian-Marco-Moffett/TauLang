#include <stdio.h>
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

#define SCAN is_eof = !scan(&last_token)

static inline void passert(TOKEN_TYPE type, const char* what) {
  if (last_token.type != type) {
    printf(PANIC "Expected \"%s\" (line %d).\n", what, last_token.line);
#if PARSE_DEBUG
    printf(PANIC "TOKEN=%d\n", last_token.type);
#endif
    panic();
  }
}

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
      panic();
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
      panic();
  }

  return NULL;
}


/*
 *  Returns an AST tree that has it's root as a binary
 *  operator.
 *
 */
struct ast_node* binexpr(void) {
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
  n = mkastnode(node_type, left, right, 0);
  return n;
}

/*
 *  Parse a compound statement.
 * 
 */

static struct ast_node* compound_statement(void) {
  struct ast_node* tree = NULL;

  /*
   *  There are no other statements
   *  for now, TODO: Add more statements.
   *
   */
  passert(TT_LBRACE, "{");
  SCAN;
  passert(TT_RBRACE, "}");
  SCAN;

  return tree;
}

/*
 *  Parse functions.
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
  g_symtbl[symbol_slot].is_global = is_global;

  passert(TT_LPAREN, "(");
  SCAN;

  passert(TT_RPAREN, ")");
  SCAN;

  struct ast_node* block_statement = compound_statement();
  return mkastunary(A_FUNC, block_statement, symbol_slot);
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
}
