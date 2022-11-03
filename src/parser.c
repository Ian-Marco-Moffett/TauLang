#include <parser.h>
#include <lexer.h>

static struct token last_token;

void parse(void) {
  scan(&last_token);
}
