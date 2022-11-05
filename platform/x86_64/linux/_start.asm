bits 64
global _start

extern f__main

_start:
  call f__main
  push rax
  mov rax, 0x3C
  pop rdi
  syscall
