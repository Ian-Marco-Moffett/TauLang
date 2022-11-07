#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <stddef.h>
#include <stdint.h>

// Structural types.
typedef enum {
  S_FUNCTION,
  S_ARGUMENT
} SYM_STYPE;

// Primitive types.
typedef enum {
  P_NONE,
  P_U8
} SYM_PTYPE;

struct symbol {
  const char* name;
  SYM_STYPE stype;
  SYM_PTYPE ptype;
  uint8_t is_global : 1;
  struct symbol* local_symtbl;
  struct symbol* parent;            // For local symbol.
  size_t local_symtbl_size;
  
  // For functions.
  size_t rbp_offset;
  size_t arg_count;
};


void init_symtbls(void);

/*
 *  Pushes global symbol to the global
 *  symbol table and returns the slot number.
 *
 */

size_t symtbl_push_glob(const char* name, SYM_STYPE stype);

/*
 *  Pushes a local symbol to a global symbol's
 *  local symbol table.
 *
 */

size_t local_symtbl_push(struct symbol* glob, const char* name, SYM_STYPE stype, SYM_PTYPE ptype);

/*
 *  Returns -1 if not found, otherwise 
 *  returns slot.
 *
 */

int64_t lookup_glob(const char* name);

extern struct symbol* g_symtbl;

#endif
