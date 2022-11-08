#include <symbol.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define ALIGN_UP(value, align)    (((value) + (align)-1) & ~((align)-1))


struct symbol* g_symtbl = NULL;
size_t g_symtbl_size = 0;


void init_symtbls(void) {
  g_symtbl = malloc(sizeof(struct symbol));
}

size_t symtbl_push_glob(const char* name, SYM_STYPE stype) {
  g_symtbl[g_symtbl_size].name = strdup(name);
  g_symtbl[g_symtbl_size].local_symtbl = NULL;
  g_symtbl[g_symtbl_size].local_symtbl_size = 0;
  g_symtbl[g_symtbl_size].rbp_offset = 0;
  g_symtbl[g_symtbl_size++].stype = stype;
  g_symtbl = realloc(g_symtbl, sizeof(struct symbol) * (g_symtbl_size + 2));
  return g_symtbl_size - 1;
}


size_t local_symtbl_push(struct symbol* glob, const char* name, SYM_STYPE stype, SYM_PTYPE ptype) {
  glob->local_symtbl[glob->local_symtbl_size].name = strdup(name);
  glob->local_symtbl[glob->local_symtbl_size].stype = stype;
  glob->local_symtbl[glob->local_symtbl_size].ptype = ptype;
  glob->local_symtbl[glob->local_symtbl_size].parent = glob;

  
  switch (ptype) {
    case P_U8:
      glob->max_rbp += 1;
  }

  // Stack must be 8 byte aligned or else
  // you get a segfault on Linux.
  glob->max_rbp = ALIGN_UP(glob->max_rbp, 8);

  glob->local_symtbl[glob->local_symtbl_size].rbp_offset = glob->max_rbp+8;
  glob->local_symtbl[glob->local_symtbl_size++].local_symtbl = NULL;
  glob->local_symtbl = realloc(glob->local_symtbl, sizeof(struct symbol) * (glob->local_symtbl_size + 2)); 

  return glob->local_symtbl_size - 1;
}


int64_t lookup_glob(const char* name) {
  for (size_t i = 0; i < g_symtbl_size; ++i) {
    if (strcmp(g_symtbl[i].name, name) == 0) {
      return i;
    }
  }

  return -1;
}


int64_t lookup_local(struct symbol* func, const char* name) {
  for (size_t i = 0; i < func->local_symtbl_size; ++i) {
    if (strcmp(func->local_symtbl[i].name, name) == 0) {
      return i;
    }
  }

  return -1;
}
