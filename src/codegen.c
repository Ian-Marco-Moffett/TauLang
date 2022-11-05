#include <codegen.h>
#include <reg.h>
#include <symbol.h>
#include <stdlib.h>
#include <unistd.h>

FILE* g_outfile = NULL;

static void prologue(void) {
  fputs(
      ";; Generated by the TauLang compiler.\n"
      ";; Copyright (c) 2022 Ian Marco Moffett, The IgnisOS team\n"
      ";; Licensed under the MIT license\n"
      ";; See LICENSE at https://github.com/Ian-Marco-Moffett/TauLang\n\n"
      "bits 64\n\n",
  g_outfile);
}


static void insert(const char* _asm) {
  fputs("\t;; User made ASM begin\n", g_outfile);
  fputs(_asm, g_outfile);
  fputs("\n\t;; User made ASM end\n", g_outfile);
}

void func_prologue(const char* name) {
  fprintf(g_outfile,
    "f__%s:\n"
    "\tpush rbp\n"
    "\tmov rbp, rsp\n", name);
}


void global(const char* name, SYM_STYPE stype) { 
  const char* stype_prefix = "u__";

  switch (stype) {
    case S_FUNCTION:
      stype_prefix = "f__";
      break;
  }

  fprintf(g_outfile, "global %s%s\n", stype_prefix, name);
}


void func_epilouge(void) {
  fputs("\tleave\n\tretq\n", g_outfile);
}


int16_t gen_code(struct ast_node* r) {
  REG leftreg, rightreg;

  switch (r->op) {
    case A_FUNC:
      if (g_symtbl[r->id].is_global)
        global(g_symtbl[r->id].name, S_FUNCTION);

      func_prologue(g_symtbl[r->id].name);
      if (r->left)
        gen_code(r->left);
      func_epilouge();
      break;
    case A_GLUE:
      gen_code(r->left);
      freeall_regs();
      gen_code(r->right);
      freeall_regs();
      return -1;
    case A_INLINE_ASM:
      if (r->text) {
        insert(r->text);
        free((char*)r->text);
        r->text = NULL;
      }
      return -1;
  }

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


void codegen_end(void) {
  fclose(g_outfile);
  char buf[450];
  snprintf(buf, sizeof(buf), "nasm -felf64 /tmp/tauout.asm -o /tmp/tauout.o && ld /tmp/tauout.o /lib/taulang/crt0.o -o a.out");
  system(buf);

  remove("/tmp/tauout.asm");
  remove("/tmp/tauout.o");
}
