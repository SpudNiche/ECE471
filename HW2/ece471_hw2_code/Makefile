CC = gcc
CFLAGS = -O2 -Wall
LFLAGS =

all:	hello_world

hello_world:	hello_world.o
	$(CC) $(LFLAGS) -o hello_world hello_world.o

hello_world.o:	hello_world.c
	$(CC) $(CFLAGS) -c hello_world.c

submit:
	tar -czvf hw2_submit.tar.gz README Makefile hello_world.c

clean:	
	rm -f *~ *.o hello_world 

