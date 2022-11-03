#include <stdio.h>
#include <stdlib.h>
#include <parser.h>


char* in_buf = NULL;
FILE* g_fp;

void compile_file(void) {
  parse();
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

  fseek(g_fp, 0, SEEK_END);
  size_t size = ftell(g_fp);
  fseek(g_fp, 0, SEEK_SET);

  in_buf = calloc(size, sizeof(char));
  fread(in_buf, sizeof(char), size, g_fp);

  compile_file();

  return 0;
}
