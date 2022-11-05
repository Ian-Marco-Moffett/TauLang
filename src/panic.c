#include <panic.h>
#include <stdlib.h>
#include <stddef.h>
#include <symbol.h>
#include <lexer.h>
#include <ast.h>

extern struct symbol* g_symtbl;
extern size_t g_symtbl_size;

void panic(void) {
  for (size_t i = 0; i < g_symtbl_size; ++i) {
    if (g_symtbl != NULL)
      free((char*)g_symtbl[i].name);
  }

  if (scanner_textbuf != NULL)
    free(scanner_textbuf);

  if (g_symtbl != NULL) {
    free(g_symtbl);
  }

  ast_destroy();
  exit(1);
}
