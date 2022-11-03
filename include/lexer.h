#ifndef LEXER_H_
#define LEXER_H_

#include <stdint.h>

typedef uint64_t INTLIT;


typedef enum {
  TT_PLUS,
  TT_MINUS,
  TT_STAR,
  TT_SLASH,
  TT_INTLIT
} TOKEN_TYPE;

struct token {
  TOKEN_TYPE type;
  INTLIT val_int;
};

uint8_t scan(struct token* t);

#endif
