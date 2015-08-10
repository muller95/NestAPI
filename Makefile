CC = gcc
CFLAGS = -Wall -g3 -lm
SRCSALL = main.c geometry.c getpoly.c gcenter.c crosscheck.c cmnfuncs.c rotnest.c cmnnest.c
BIN = test
OBJS = $(SRCSALL:.c=.o)

all: terminp geometry polygon gcenter cmnfuncs crosscheck rotnest
	$(CC) $(CFLAGS) -o $(BIN) $(OBJS)
	
terminp: main.c cmnfuncs
	$(CC) $(CFLAGS) -c -o main.o main.c

rotnest: cmnnest geometry rotnest.c
	$(CC) $(CFLAGS) -c -o rotnest.o rotnest.c

cmnnest: cmnnest.c
	$(CC) $(CFLAGS) -c -o cmnnest.o cmnnest.c

gcenter: cmnfuncs
	$(CC) $(CFLAGS) -c -o gcenter.o gcenter.c

geometry: geometry.c cmnfuncs
	$(CC) $(CFLAGS) -c -o geometry.o geometry.c

polygon: getpoly.c cmnfuncs
	$(CC) $(CFLAGS) -c -o getpoly.o getpoly.c

crosscheck: crosscheck.c cmnfuncs
	$(CC) $(CFLAGS) -c -o crosscheck.o crosscheck.c

cmnfuncs: cmnfuncs.c
	$(CC) $(CFLAGS) -c -o cmnfuncs.o cmnfuncs.c

	


clean:
	rm -rf *.sw? *.o
