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

static struct ast_node* binexpr(void);
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


/*
 *  Parse a function call.
 *  
 *  identifier()
 *
 */

static struct ast_node* func_call(size_t slot) {
  if (g_symtbl[slot].stype != S_FUNCTION) {
    printf(PANIC "Trying to call \"%s\" which is not a function! (line %d)\n", g_symtbl[slot].name, last_token.line);
    exit(1);
  }

  passert(TT_LPAREN, "(");
  SCAN;

  struct ast_node* arg_tree = NULL;
  struct ast_node* left = NULL;
  size_t arg_n = 0;
  
  while (1) {
    if (last_token.type == TT_RPAREN)
      break;

    if (arg_n+1 > g_symtbl[slot].arg_count) {
      printf(PANIC "Too many arguments supplied for function \"%s\" (line %d)\n", g_symtbl[slot].name, last_token.line);
      exit(1);
    } 

    if (left == NULL) {
      left = mkastnode(A_ARG_PASS, NULL, binexpr(), NULL, arg_n);
      arg_tree = left;
    } else {
      left->left = mkastnode(A_ARG_PASS, NULL, binexpr(), NULL, arg_n);
      left = left->left;
    }

    if (last_token.type == TT_RPAREN) {
      SCAN;
      break;
    } else if (last_token.type != TT_COMMA) {
      printf(PANIC "Expected ',' or ')' (line %d)\n", last_token.line);
      exit(1);
    }

    SCAN;
    ++arg_n;
  } 

  if (arg_n+1 < g_symtbl[slot].arg_count) {
      printf(PANIC "Too few arguments supplied for function \"%s\" (line %d)\n", g_symtbl[slot].name, last_token.line);
      exit(1);
  }
  
  passert(TT_SEMI, ";");
  SCAN;

  return mkastunary(A_FUNCCALL, arg_tree, slot);
}



static struct ast_node* identifier(void) {
  size_t slot = lookup_glob(scanner_idbuf);
  size_t local_slot = lookup_local(&g_symtbl[current_func_id], scanner_idbuf);

  if (slot == -1 && local_slot == -1) {
    printf(PANIC "Symbol \"%s\" not found (line %d)\n", scanner_idbuf, last_token.line);
    exit(1);
  }

  if (local_slot != -1) {
    return mkastleaf(A_LOCAL_VAR, local_slot);
  }

  SCAN;

  if (last_token.type == TT_LPAREN) {
    return func_call(slot);
  }

  printf(PANIC "Syntax error (line %d)\n", last_token.line);
  exit(1);
  __builtin_unreachable();
}


static struct ast_node* primary_factor(void) {
  struct ast_node* n;

  switch (last_token.type) {
    case TT_INTLIT:
      n = mkastleaf(A_INTLIT, last_token.val_int);
      SCAN;
      return n;
    case TT_ID:
      n = identifier();
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

  if (is_eof || last_token.type == TT_RPAREN || last_token.type == TT_SEMI || last_token.type == TT_COMMA)
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

  /*
   *  got_return is used
   *  for checking to see
   *  if the function has
   *  a return statement.
   *
   *  This is to avoid 
   *  functions that say they
   *  return a value but lack
   *  a return statement.
   *
   *
   */

  uint8_t got_return = 0;

  passert(TT_LBRACE, "{");
  SCAN;

  while (1) {
    if (last_token.type == TT_RBRACE) {
      /*
       *  Ensure if the function returns
       *  a non none type then it must
       *  have a return statement.
       *
       */
      if (g_symtbl[current_func_id].ptype != P_NONE && !(got_return)) {
        printf(PANIC "Function \"%s\" lacks return statement and doesn't have type none (line %d).\n", g_symtbl[current_func_id].name, last_token.line);
        exit(1);
      }

      SCAN;
      return left;
    }

    if (got_return) {
      printf(PANIC "Expected '}' after return statement (line %d)\n", last_token.line);
      exit(1);
    }

    switch (last_token.type) {
      case TT_ASM:
        tree = inline_assembly();
        break;
      case TT_RETURN:
        tree = return_statement();
        got_return = 1;
        break;
      case TT_ID:
        tree = identifier();
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
 *  or
 *
 *  symbol(u8 arg0, u8 arg1) -> type
 *
 *  or
 *
 *  symbol(u8 arg0) -> type
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

  /*
   *  Setup the local symbol table 
   *  for this function.
   *
   */

  g_symtbl[symbol_slot].local_symtbl = malloc(sizeof(struct symbol));

  /*
   *  Now what we will do
   *  is begin the parsing
   *  of the function
   *  arguments.
   *
   */
  passert(TT_LPAREN, "(");
  SCAN; 

  struct ast_node* arg_tree = NULL;
  struct ast_node* left = NULL;
  SYM_PTYPE current_ptype;

  size_t arg_n = 0;
  while (1) {
    if (last_token.type == TT_RPAREN)
      break;

    g_symtbl[symbol_slot].arg_count++;

    switch (last_token.type) {
      case TT_U8:
        current_ptype = P_U8;
        break;
      case TT_NONE:
        printf(PANIC "Argument %d cannot be of type none (line %d).\n", arg_n, last_token.line);
        exit(1);
        break;
      default:
        printf(PANIC "Expected type for argument %d (line %d).\n", arg_n, last_token.line);
        exit(1);
    }

    SCAN;

    if (last_token.type != TT_ID) {
      printf(PANIC "Expected identifier for argument %d (line %d).\n", arg_n, last_token.line);
      exit(1);
    }
  
    /*
     *  Push this argument
     *  to the function's
     *  local symbol table
     *  and create an 
     *  AST node for this
     *  argument.
     *
     */
    size_t local_sym_id = local_symtbl_push(&g_symtbl[symbol_slot], scanner_idbuf, S_ARGUMENT, current_ptype);
   
    if (left == NULL) {
      left = mkastleaf(A_ARG, local_sym_id);
      left->extra_argument = arg_n;
      arg_tree = left;
    } else {
      left->left = mkastleaf(A_ARG, local_sym_id);
      left = left->left;
      left->extra_argument = arg_n;
    }

    SCAN;

    if (last_token.type == TT_COMMA) {
      ++arg_n;
      SCAN;
      continue;
    } else if (last_token.type == TT_RPAREN) {
      break;
    } else {
      printf(PANIC "Syntax error (line %d).\n", last_token.line);
      exit(1);
    }
  }

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
      printf(PANIC "Invalid function type (line %d).\n", last_token.line);
      exit(1);
  }

  SCAN;

  struct ast_node* block_statement = compound_statement();
  return mkastnode(A_FUNC, block_statement, NULL, arg_tree, symbol_slot);
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
      gen_code(tree, NULL);
    else
      break;
  }

  codegen_end();
}
