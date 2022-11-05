#ifndef REG_H_
#define REG_H_

#include <def.h>
#include <codegen.h>

typedef uint16_t REG;

REG reg_load(INTLIT value);
REG reg_add(REG r1, REG r2);
REG reg_mul(REG r1, REG r2);
REG reg_sub(REG r1, REG r2);
REG reg_div(REG r1, REG r2);
void freeall_regs(void);


extern const char* rregs[4];
extern const char* bregs[4];

#endif
