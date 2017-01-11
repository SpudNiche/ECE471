CC = gcc
CFLAGS = -O2 -Wall
LFLAGS =


all:	display_temp

###

display_temp:	display_temp.o
	$(CC) $(LFLAGS) -o display_temp display_temp.o

display_temp.o:	display_temp.c
	$(CC) $(CFLAGS) -c display_temp.c


submit:	display_temp.c README Makefile
	tar -czvf hw10_submit.tar.gz display_temp.c fakedate README Makefile

clean:	
	rm -f *~ *.o display_temp display_final display_cool

