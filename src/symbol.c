#include <symbol.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

struct symbol* g_symtbl = NULL;
size_t g_symtbl_size = 0;


void init_symtbls(void) {
  g_symtbl = malloc(sizeof(struct symbol));
}

size_t symtbl_push_glob(const char* name, SYM_STYPE stype) {
  g_symtbl[g_symtbl_size].name = strdup(name);
  g_symtbl[g_symtbl_size++].stype = stype;
  g_symtbl = realloc(g_symtbl, sizeof(struct symbol) * (g_symtbl_size + 2));
  return g_symtbl_size - 1;
}


int64_t lookup_glob(const char* name) {
  for (size_t i = 0; i < g_symtbl_size; ++i) {
    if (strcmp(g_symtbl[i].name, name) == 0) {
      return i;
    }
  }

  return -1;
}
