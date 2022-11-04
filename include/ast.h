#ifndef AST_H_
#define AST_H_

#include <def.h>

typedef enum {
  A_ADD,
  A_SUB,
  A_MUL,
  A_DIV,
  A_INTLIT
} NODE_TYPE;


struct ast_node {
  NODE_TYPE op;
  struct ast_node* left;
  struct ast_node* right;
  INTLIT val_int; 
};

void ast_destroy(void);
struct ast_node* mkastnode(NODE_TYPE op, struct ast_node* left, struct ast_node* right, INTLIT val_int);
struct ast_node* mkastleaf(NODE_TYPE op, INTLIT val_int);
struct ast_node* mkastunary(NODE_TYPE op, struct ast_node* left, INTLIT val_int);

#endif