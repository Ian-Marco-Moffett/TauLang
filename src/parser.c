#include <stdio.h>
#include <parser.h>
#include <lexer.h>
#include <def.h>
#include <panic.h>
#include <ast.h>

#define PARSE_DEBUG 0

static struct token last_token;
static uint8_t is_eof = 0;

#define SCAN is_eof = !scan(&last_token)

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

  if (is_eof)
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

INTLIT test_ast(struct ast_node* tree) {
  INTLIT leftval, rightval;

  if (tree->left)
    leftval = test_ast(tree->left);

  if (tree->right)
    rightval = test_ast(tree->right);

  switch (tree->op) {
    case A_ADD:
      return leftval + rightval;
    case A_SUB:
      return leftval - rightval;
    case A_MUL:
      return leftval * rightval;
    case A_DIV:
      return leftval / rightval;
    case A_INTLIT:
      return tree->val_int;
  }

  return 0;
}

void parse(void) {
  SCAN;
  while (!(is_eof)) {
    struct ast_node* tree = binexpr();
    printf("RESULT=%d\n", test_ast(tree));
  }
}
