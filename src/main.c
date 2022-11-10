#include <stdio.h>
#include <stdlib.h>
#include <parser.h>
#include <ast.h>
#include <panic.h>
#include <stdlib.h>
#include <stddef.h>
#include <symbol.h>
#include <lexer.h>
#include <ast.h>
#include <argp.h>

extern struct symbol* g_symtbl;
extern size_t g_symtbl_size;

size_t flags = 0;
static const char* input_file = NULL;
const char* output_file = NULL;

static struct argp_option options[] = {
  {"in", 'i', "INPUT", 0, "Input file to compile"},
  {"out", 'o', "OUTPUT", 0, "Output file (example: a.out)"},
  {"objonly", 'c', 0, 0, "Output an object file only"},
  {0}
};

static error_t parse_opt(int key, char* arg, struct argp_state* state) {
  switch (key) {
    case 'i':
      input_file = arg;
      break;
    case 'o':
      output_file = arg;
      break;
    case 'c':
      flags |= FLAG_OBJONLY;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}


static struct argp argp = {options, parse_opt, 0, 0, 0, 0, 0};

static void cleanup(void) {
  for (size_t i = 0; i < g_symtbl_size; ++i) {
    if (g_symtbl != NULL) {
      free((char*)g_symtbl[i].name);
    }

    for (size_t j = 0; j < g_symtbl[i].local_symtbl_size; ++j) {
      if (g_symtbl[i].local_symtbl[j].name != NULL)
        free((char*)g_symtbl[i].local_symtbl[j].name);
    }

    if (g_symtbl[i].local_symtbl != NULL) {
      free(g_symtbl[i].local_symtbl);
    }
  }

  if (scanner_textbuf != NULL)
    free(scanner_textbuf);

  if (g_symtbl != NULL) {
    free(g_symtbl);
  }

  ast_destroy();
  exit(1);
}


FILE* g_fp;

static void compile(void) {
  parse();
}


static void _on_exit(void) {
  fclose(g_fp);
  
  cleanup();
}


int main(int argc, char** argv) {
  if (argc < 2) {
    printf("Error: Too few arguments!\n");
    return 1;
  }
  argp_parse(&argp, argc, argv, 0, 0, 0);

  if (input_file == NULL) {
    printf("Error: Too few arguments!\n");
    return 1;
  }

  g_fp = fopen(input_file, "r");

  if (g_fp == NULL) {
    perror("Error");
    return 1;
  }

  atexit(_on_exit);
  compile();

  return 0;
}
