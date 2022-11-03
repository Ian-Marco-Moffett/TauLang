#include <lexer.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <panic.h>

#define ISDIGIT(n) (n >= '0' && n <= '9')

extern char* in_buf;
static size_t in_buf_idx = 0;
static size_t line = 1;

static void skip(void) { 
  char c = in_buf[in_buf_idx];
  while (c == '\t' || c == '\f' || c == '\r' || c == '\n') {
    if (c == '\n') {
      ++line;
    }

    c = in_buf[in_buf_idx++];
  }
}

static char next(void) { 
  if (in_buf[in_buf_idx] == '\0') {
    return EOF;
  }

  return in_buf[in_buf_idx++];
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

  buf[buf_idx] = '\0';
  return atoi(buf);
}


uint8_t scan(struct token* t) {
  skip();
  char c = next();

  switch (c) {
    case EOF:
      return 0;
    case '+':
      return TT_PLUS;
    case '-':
      return TT_MINUS;
    case '*':
      return TT_STAR;
    case '/':
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
