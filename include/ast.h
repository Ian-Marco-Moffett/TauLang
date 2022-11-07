#ifndef AST_H_
#define AST_H_

#include <def.h>
#include <stddef.h>

typedef enum {
  A_ADD,
  A_SUB,
  A_MUL,
  A_DIV,
  A_INTLIT,
  A_FUNC,
  A_INLINE_ASM,
  A_GLUE,
  A_RETURN,
  A_FUNCCALL,
  A_ARG,
} NODE_TYPE;


struct ast_node {
  NODE_TYPE op;
  struct ast_node* left;
  struct ast_node* mid;
  struct ast_node* right;
  const char* text;
  size_t extra_argument;            // Used for stuff like counting arguments.

  union {
    INTLIT val_int; 
    uint64_t id;
  };
};

void ast_destroy(void);
struct ast_node* mkastnode(NODE_TYPE op, struct ast_node* left, struct ast_node* mid, struct ast_node* right, INTLIT val_int);
struct ast_node* mkastleaf(NODE_TYPE op, INTLIT val_int);
struct ast_node* mkastunary(NODE_TYPE op, struct ast_node* left, INTLIT val_int);

#endif
