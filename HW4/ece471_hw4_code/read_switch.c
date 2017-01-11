// File: read_switch.c
// ECE 471 - Homework 4
// Author: Nicholas LaJoie 

#include <stdio.h>

#include <fcntl.h>	/* open() */
#include <unistd.h>	/* close() */
#include <string.h>

int main(int argc, char **argv) {

	// Enable GPIO Pin 17
	int fd = open ("/sys/class/gpio/export", O_WRONLY);
	write (fd, "17", 2); 
	close (fd);

	// Set GPIO Pin 17 as an Input
	fd = open("/sys/class/gpio/gpio17/direction", O_WRONLY);
	write (fd, "in", 2); 
	close (fd);

	// Read initial value of GPIO Pin 17
	char buffer[16]; 
	fd = open ("/sys/class/gpio/gpio17/value", O_RDONLY);
        read (fd, buffer, 16);
        close (fd);

	// Infinite loop polling if GPIO Pin 17 is receiving an input signal
	// This loop uses simple software debouncing - waits until signal
	// changes from 0 to 1 to print "Pressed" and vice versa 
	while (1) {
		while (buffer[0] == '0') {
			fd = open ("/sys/class/gpio/gpio17/value", O_RDONLY);
        		read (fd, buffer, 16);
        		close (fd);
		}
		printf("Key Pressed!\n");
		while (buffer[0] == '1') {
			fd = open ("/sys/class/gpio/gpio17/value", O_RDONLY);
        		read (fd, buffer, 16);
        		close (fd);
		}
		printf("Key Released!\n"); 
	}

	return 0;
}

