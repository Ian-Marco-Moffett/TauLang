CC = gcc
CFLAGS = -Werror=implicit -Werror=implicit-function-declaration -Werror=implicit-int \
  			 -Werror=int-conversion \
  			 -Werror=incompatible-pointer-types -Werror=int-to-pointer-cast -Werror=return-type -Wunused -Iinclude -fsanitize=address

CFILES = $(shell find src/ -name "*.c")

bin/tau: $(CFILES)
	@ mkdir -p bin/
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: install_linux
install_linux:
	sudo mkdir -p /lib/taulang/
	sudo nasm -felf64 platform/x86_64/linux/_start.asm -o /lib/taulang/crt0.o
	sudo cp bin/tau /usr/local/bin/tau
