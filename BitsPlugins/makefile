CFLAGS_OPTIMIZATION = -g
#CFLAGS_OPTIMIZATION = -O3
CFLAGS_VERSION = -std=c11
CFLAGS_WARNINGS = -Wall -Wextra -Wno-unused-parameter # -Wno-unused-but-set-parameter -Wno-unused-result
CFLAGS_DEFINES = -D_XOPEN_SOURCE=700 -D_POSIX_C_SOURCE=200809L -D_POSIX_SOURCE=1 -D_DEFAULT_SOURCE=1 -D_GNU_SOURCE=1
CFLAGS = $(CFLAGS_OPTIMIZATION) $(CFLAGS_VERSION) $(CFLAGS_WARNINGS) $(CFLAGS_DEFINES)

CC = gcc
RM = rm -rf

all:
	@echo "Select between: 'make linux' or 'make aix' libs"

linux: clean
	$(CC) -Wall -fPIC -c bitEspecificationA.c -I../include $(CFLAGS)
	$(CC) -shared -Wl,-soname,bitEspecificationA.so.1 -o bitEspecificationA.so bitEspecificationA.o
	$(RM) bitEspecificationA.o

aix: clean
	cc -c bitEspecificationA.c -o bitEspecificationA.o -I../include
	ar -rv bitEspecificationA.a bitEspecificationA.o
	xlc -G -o bitEspecificationA.so bitEspecificationA.o

clean:
	-$(RM) *.o *.so
