CC = gcc
CFLAGS = -Werror=implicit -Werror=implicit-function-declaration -Werror=implicit-int \
  			 -Werror=int-conversion \
  			 -Werror=incompatible-pointer-types -Werror=int-to-pointer-cast -Werror=return-type -Wunused -fsanitize=address -Iinclude

CFILES = $(shell find src/ -name "*.c")

bin/tau: $(CFILES)
	@ mkdir -p bin/
	$(CC) $(CFLAGS) $^ -o $@
