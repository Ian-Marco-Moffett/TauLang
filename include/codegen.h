#ifndef CODEGEN_H_
#define CODEGEN_H_

#include <ast.h>
#include <stdio.h>
#include <stdint.h>

uint16_t gen_code(struct ast_node* r);
void codegen_init(void);
void epilouge(void);

extern FILE* g_outfile;


#endif
