CC = gcc
CFLAGS_LIB = -Wall -g3 -lm -fPIC
CFLAGS_PROG = -Wall -g3 -I./include -L. -lnestapi -lpthread
SRCS_PROG = example/main.c
SRCS_LIB = geometry.c getpoly.c crosscheck.c cmnfuncs.c rotnest.c cmnnest.c mtxnest.c
BIN = example/test
OBJS_LIB = $(SRCS_LIB:.c=.o)

all: lib single multi test

test:
	$(CC) $(CFLAGS_PROG) -o mtxttest example/mtxtest.c

single:
	$(CC) $(CFLAGS_PROG) -o snest example/main.c

multi:
	$(CC) $(CFLAGS_PROG) -o mtnest example/mainmt.c


lib: objs
	$(CC) $(CFLAGS_LIB) -shared -o libnestapi.so $(OBJS_LIB)

objs:
	$(CC) $(CFLAGS_LIB) -c $(SRCS_LIB)


clean:
	rm -rf *.sw? *.o
