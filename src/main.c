#include <stdio.h>
#include <stdlib.h>
#include <parser.h>

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


  compile_file();

  return 0;
}
