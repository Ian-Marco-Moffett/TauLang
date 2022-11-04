#include <lexer.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <panic.h>

#define ISDIGIT(n) (n >= '0' && n <= '9')
#define TOLOWER(x) (x | (1 << 5))
#define ISALPHA(x) (TOLOWER(x) >= 'a' && TOLOWER(x) <= 'z')


extern FILE* g_fp;
static size_t line = 1;
static char spared = '\0';
char scanner_idbuf[MAX_ID_LENGTH];

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

/*
 *  Scan an identifier into a buffer.
 *
 */

static void scan_id(char c, char* buf) {
  size_t buf_idx = 0;

  while (1) {
    if (c != '_' && !(ISALPHA(c))) {
      break;
    }

    if (buf_idx >= MAX_ID_LENGTH-2) {
      printf(PANIC "Identifier max length reached (line %d)\n", line);
      panic();
    }
  
    buf[buf_idx++] = c;
    c = next();
  }

  spare(c);
  buf[buf_idx] = '\0';
}


static void keyword(struct token* t) {
  switch (scanner_idbuf[0]) {
    case 'g':
      if (strcmp(scanner_idbuf, "global") == 0) { 
        t->type = TT_GLOBAL;
        return;
      }
      
      t->type = TT_ID;
      break;
    default:
      t->type = TT_ID;
      break;
  }
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

  t->line = line;

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
      return 1;
    case '(':
      t->type = TT_LPAREN;
      return 1;
    case ')':
      t->type = TT_RPAREN;
      return 1;
    case ';':
      t->type = TT_SEMI;
      return 1;
    case '{':
      t->type = TT_LBRACE;
      return 1;
    case '}':
      t->type = TT_RBRACE;
      return 1;
    default:
      if (ISDIGIT(c)) {
        t->val_int = scanint(c);
        t->type = TT_INTLIT;
        return 1;
      } else if (ISALPHA(c)) {
        scan_id(c, scanner_idbuf);
        keyword(t);
        return 1;
      }

      printf(PANIC "Invalid character \"%c\" while scanning (line %d)\n", c, line);
      panic();
  }

  return 0;
}
