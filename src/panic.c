#include <panic.h>
#include <stdlib.h>
#include <stddef.h>
#include <symbol.h>

extern struct symbol* g_symtbl;
extern size_t g_symtbl_size;

void panic(void) {
  for (size_t i = 0; i < g_symtbl_size; ++i) {
    if (g_symtbl != NULL)
      free((char*)g_symtbl[i].name);
  }

  if (g_symtbl != NULL) {
    free(g_symtbl);
  }
  exit(1);
}
