CC = gcc
CFLAGS_PROG = -Wall -g3 -lm -L. -lnestapi -I./include
CFLAGS_LIB = -Wall -g3 -lm -fPIC
SRCS_PROG = example/main.c
SRCS_LIB = geometry.c getpoly.c crosscheck.c cmnfuncs.c rotnest.c cmnnest.c
BIN = example/test
OBJS_LIB = $(SRCS_LIB:.c=.o)

all: lib
	$(CC) $(CFLAGS_PROG) -o $(BIN) $(SRCS_PROG)	

lib: objs
	$(CC) $(CFLAGS_LIB) -shared -o libnestapi.so $(OBJS_LIB)

objs:
	$(CC) $(CFLAGS_LIB) -c $(SRCS_LIB)


clean:
	rm -rf *.sw? *.o
