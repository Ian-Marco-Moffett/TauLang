bits 64
global _start

extern main

_start:
  call main
  push rax
  mov rax, 0x3C
  pop rdi
  syscall
