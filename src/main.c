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

extern struct symbol* g_symtbl;
extern size_t g_symtbl_size;

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

  // First (argv[1]) argument is filename.
  g_fp = fopen(argv[1], "r");

  if (g_fp == NULL) {
    perror("Error");
    return 1;
  }

  atexit(_on_exit);
  compile();

  return 0;
}
