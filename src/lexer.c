#include <lexer.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <panic.h>

#define ISDIGIT(n) (n >= '0' && n <= '9')

extern FILE* g_fp;
static size_t line = 1;

static char next(void) {
  char c;

  do {
    c = fgetc(g_fp);

    if (c == '\n') {
      ++line;
    }
  } while (c == '\f' || c == '\t' || c == '\r' || c == '\n' || c == ' ');

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

  buf[buf_idx] = '\0';
  return atoi(buf);
}


uint8_t scan(struct token* t) {
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
        printf("%d\n", t->val_int);
        return 1;
      }

      printf(PANIC "Invalid character \"%c\" while scanning (line %d)\n", line);
      panic();
      return 1;
  }
}
