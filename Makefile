CC=gcc
AR=ar
CFLAGS=-Wall -g

BINS= xthreads.a 

all: $(BINS)

xthreads.a:
	$(CC) $(CFLAGS) -c xthreads.c ds/linkedlist.c ds/list.c ds/queue.c 
	$(AR) -cvr xthreads.a xthreads.o linkedlist.o list.o queue.o

clean:
	rm $(BINS) *.o 
