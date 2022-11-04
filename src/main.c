#include <stdio.h>
#include <stdlib.h>
#include <parser.h>


FILE* g_fp;

static void compile(void) {
  parse();
}


static void _on_exit(void) {
  extern FILE* g_outfile;
  fclose(g_fp);

  if (g_outfile != NULL)
    fclose(g_outfile);
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
