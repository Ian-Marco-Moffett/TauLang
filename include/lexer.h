#ifndef LEXER_H_
#define LEXER_H_

#include <stdint.h>
#include <stddef.h>
#include <def.h>

#define MAX_ID_LENGTH 40

typedef enum {
  TT_PLUS,
  TT_MINUS,
  TT_STAR,
  TT_SLASH,
  TT_INTLIT,
  TT_GLOBAL,
  TT_ID,
  TT_LPAREN,
  TT_RPAREN,
  TT_SEMI,
  TT_LBRACE,
  TT_RBRACE,
} TOKEN_TYPE;

struct token {
  TOKEN_TYPE type;
  INTLIT val_int;
  size_t line;
};

uint8_t scan(struct token* t);

extern char scanner_idbuf[MAX_ID_LENGTH];

#endif
