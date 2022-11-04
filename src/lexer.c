#include <lexer.h>
#include <stdlib.h>
#include <stdio.h>
#include <panic.h>

#define ISDIGIT(n) (n >= '0' && n <= '9')

extern FILE* g_fp;
static size_t line = 1;
static char spared = '\0';

static inline void spare(char c) {
  spared = c;
}

static char next(void) { 
  char c = fgetc(g_fp); 

  if (c == '\0') {
    return EOF;
  }

  return c;
}

static INTLIT scanint(char c) {
  char buf[21];
  size_t buf_idx = 0;

  do {
    if (buf_idx >= sizeof(buf)-1) {
      printf(PANIC "Too many digits for integer literal! (line %d)\n", line);
      panic();
    }

    buf[buf_idx++] = c;
    c = next();
  } while (ISDIGIT(c));

  spare(c);  

  buf[buf_idx] = '\0';
  return atoi(buf);
}


uint8_t scan(struct token* t) {
  char c;
  if (spared == '\0') {
    c = next();
  } else {
    c = spared;
    spared = '\0';
  }

  while (c == '\t' || c == '\r' || c == '\n' || c == ' ') {
    c = next();
  }

  switch (c) {
    case EOF:
      return 0;
    case '+':
      t->type = TT_PLUS;
      return 1;
    case '-':
      t->type = TT_MINUS;
      return 1;
    case '*':
      t->type = TT_STAR;
      return 1;
    case '/':
      t->type = TT_SLASH;
      return TT_SLASH;
    default:
      if (ISDIGIT(c)) {
        t->val_int = scanint(c);
        t->type = TT_INTLIT;
        return 1;
      }

      printf(PANIC "Invalid character \"%c\" while scanning (line %d)\n", c, line);
      panic();
  }

  return 0;
}
