#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <stddef.h>
#include <stdint.h>

// Structural types.
typedef enum {
  S_FUNCTION
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
};


void init_symtbls(void);

/*
 *  Pushes global symbol to the global
 *  symbol table and returns the slot number.
 *
 */

size_t symtbl_push_glob(const char* name, SYM_STYPE stype);

/*
 *  Returns -1 if not found, otherwise 
 *  returns slot.
 *
 */

int64_t lookup_glob(const char* name);

extern struct symbol* g_symtbl;

#endif
