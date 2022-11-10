#ifndef DEF_H_
#define DEF_H_

#include <stdint.h>
#include <stddef.h>

#define FLAG_OBJONLY (1 << 0)


typedef uint64_t INTLIT;
extern const char* output_file;
extern size_t flags;

#endif
