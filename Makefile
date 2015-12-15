CC = gcc
CFLAGS_LIB = -Wall -lm -fPIC
CFLAGS_PROG = -Wall -I./include -L. -lnestapi -lpthread
SRCS_PROG = example/main.c
SRCS_LIB = geometry.c getpoly.c crosscheck.c cmnfuncs.c rotnest.c cmnnest.c mtxnest.c
BIN = example/test
OBJS_LIB = $(SRCS_LIB:.c=.o)

all: lib single multi dbgnest mtxtest

dbgnest: objs
	$(CC) -Wall -g3 -I./include -lpthread -lm -o dbgnest example/mainmt.c $(SRCS_LIB)

mtxtest: objs
	$(CC) -Wall -g3 -I./include -lpthread -lm -o mtxtest example/mtxtest.c $(SRCS_LIB)

single: objs
	$(CC) $(CFLAGS_PROG) -o snest example/main.c

multi: objs
	$(CC) $(CFLAGS_PROG) -o mtnest example/mainmt.c


lib: objs
	$(CC) $(CFLAGS_LIB) -shared -o libnestapi.so $(OBJS_LIB)

objs:
	$(CC) $(CFLAGS_LIB) -c $(SRCS_LIB)


install:
	cp mtnest /usr/bin/mtnest
	cp mtxtest /usr/bin/mtxtest
	cp libnestapi.so /usr/lib/libnestapi.so
	
clean:
	rm -rf *.sw? *.o
