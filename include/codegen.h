#ifndef CODEGEN_H_
#define CODEGEN_H_

#include <ast.h>
#include <stdio.h>
#include <stdint.h>

int16_t gen_code(struct ast_node* r);
void codegen_insert(const char* _asm);
void codegen_init(void);

extern FILE* g_outfile;


#endif
