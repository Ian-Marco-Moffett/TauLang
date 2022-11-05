#include <reg.h>
#include <codegen.h>
#include <stdint.h>


const char* rregs[4] = {"r8", "r9", "r10", "r11"};
const char* bregs[4] = {"r8b", "r9b", "r10b", "r11b"};
static uint8_t regbmp = 0xFF;


static REG alloc_reg(void) {
  for (int i = 0; i < sizeof(regbmp)*8; ++i) {
    if (regbmp & (1 << i)) {
      regbmp &= ~(1 << i);
      return i;
    }
  }

  return -1;
}


static void free_reg(REG r) {
  if (r > (sizeof(regbmp)*8) - 1 || r < 0) {
    return;
  }

  regbmp |= (1 << r);
}


void freeall_regs(void) {
  regbmp = 0xFF;
}


REG reg_load(INTLIT value) {
  REG r = alloc_reg();
  fprintf(g_outfile, "\tmov %s, %d\n", rregs[r], value);
  return r;
}


REG reg_add(REG r1, REG r2) {
  fprintf(g_outfile, "\tadd %s, %s\n", rregs[r1], rregs[r2]);
  free_reg(r2);
  return r1;
}


REG reg_sub(REG r1, REG r2) {
  fprintf(g_outfile, "\tsub %s, %s\n", rregs[r1], rregs[r2]);
  free_reg(r2);
  return r1;
}

REG reg_mul(REG r1, REG r2) {
  fprintf(g_outfile, "\timul %s, %s\n", rregs[r1], rregs[r2]);
  free_reg(r2);
  return r1;
}

REG reg_div(REG r1, REG r2) {
  fprintf(g_outfile, "\tmov rax, %s\n", rregs[r1]);
  fprintf(g_outfile, "\tcqo\n");
  fprintf(g_outfile, "\tidiv %s\n", rregs[r2]);
  fprintf(g_outfile, "\tmov %s, rax\n", rregs[r1]);
  free_reg(r2);
  return r1;
}

void reg_printint(REG r) {
  fprintf(g_outfile, "\tmov rdi, %s\n", rregs[r]);
  fprintf(g_outfile, "\tcall printint\n");
  free_reg(r);
}
