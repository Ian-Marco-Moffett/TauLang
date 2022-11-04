#include <codegen.h>
#include <reg.h>

FILE* g_outfile = NULL;

static void prologue(void) {
  fputs(
    "extern printf\n\n"
    "section .data\n"
    "integer: db \"%d\", 0xA, 0\n"
    "string: db \"%s\", 0xA, 0\n\n"
    "section .text\n" 
    "printint:\n"
    "\tmov rsi, rdi\n"
    "\tmov rdi, integer\n"
    "\tpush rsp\n"
    "\tcall [rel printf wrt ..got]\n"
    "\tpop rsp\n"
    "\tret\n\n"
    "printstr:\n"
    "\tmov rsi, rdi\n"
    "\tmov rdi, string\n"
    "\tpush rsp\n"
    "\tcall [rel printf wrt ..got]\n"
    "\tpop rsp\n"
    "\tret\n"
    "\n"
    "main:\n"
    "\tpush rbp\n"
    "\tmov rsp, rbp\n",
  g_outfile);
}

void epilouge(void) {
  fputs(
    "\tmov rax, 0\n"
    "\tleave\n"
    "\tret\n", g_outfile);
}


uint16_t gen_code(struct ast_node* r) {
  REG leftreg, rightreg;

  if (r->left)
    leftreg = gen_code(r->left);

  if (r->right)
    rightreg = gen_code(r->right);

  switch (r->op) {
    case A_ADD:
      return reg_add(leftreg, rightreg);
    case A_SUB:
      return reg_sub(leftreg, rightreg);
      break;
    case A_MUL:
      return reg_mul(leftreg, rightreg);
    case A_DIV:
      return reg_div(leftreg, rightreg);
    case A_INTLIT:
      return reg_load(r->val_int);
  }

  return 0;
}

void codegen_init(void) {
  g_outfile = fopen("/tmp/tauout.asm", "w");
  prologue();
}
