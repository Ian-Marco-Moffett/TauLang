#include <ast.h>
#include <panic.h>
#include <stdlib.h>
#include <stdio.h>

static struct ast_node** nodes = NULL;
static size_t nodes_idx = 0;

static inline void push(struct ast_node* n) {
  nodes[nodes_idx++] = n;
  nodes = realloc(nodes, sizeof(struct ast_node*) * (nodes_idx+1));
}

void ast_destroy(void) {
  if (nodes == NULL)
    return;

  for (size_t i = 0; i < nodes_idx; ++i) {
    free(nodes[i]);
  }

  free(nodes);
  nodes = NULL;
}

struct ast_node* mkastnode(NODE_TYPE op, struct ast_node* left, struct ast_node* right, INTLIT val_int) {
  if (nodes == NULL) {
    nodes = malloc(sizeof(struct ast_node*));
    if (nodes == NULL) {
      printf(PANIC "__INTERNAL_ERROR__: could not allocate memory for nodes in %s()\n", __func__);
      panic();
    }
  }

  struct ast_node* n = malloc(sizeof(struct ast_node));
  
  if (n == NULL) {
    printf(PANIC "__INTERNAL_ERROR__: could not allocate memory for node in %s()\n", __func__);
    panic();
  }

  n->op = op;
  n->left = left;
  n->right = right;
  n->val_int = val_int;
  push(n);
  return n;
}

struct ast_node* mkastleaf(NODE_TYPE op, INTLIT val_int) {
  return mkastnode(op, NULL, NULL, val_int);
}

struct ast_node* mkastunary(NODE_TYPE op, struct ast_node* left, INTLIT val_int) {
  return mkastnode(op, left, NULL, val_int);
}
