#ifndef CODEGEN_H_
#define CODEGEN_H_

#include <ast.h>
#include <stdio.h>
#include <stdint.h>

int16_t gen_code(struct ast_node* r, struct ast_node* r1);
void codegen_insert(const char* _asm);
void codegen_init(void);
void codegen_end(void);

extern FILE* g_outfile;


#endif
