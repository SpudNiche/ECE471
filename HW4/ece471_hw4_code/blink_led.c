// ECE 471 - HW 4
// Author: Nicholas LaJoie
// File: blink_led.c

#include <stdio.h>

#include <fcntl.h>	/* open() */
#include <unistd.h>	/* close() */
#include <string.h>


int main(int argc, char **argv) {

	// Blink an LED at 1 Hz with a 50% duty cycle

	// Enable GPIO 18
	char buffer[10];
	int fd;  
	fd=open("/sys/class/gpio/export", O_WRONLY); 
	if (fd == -1) fprintf(stderr, "\tError enabling\n"); // Error Checking 
	strcpy(buffer,"18"); 
	write(fd,buffer,2); 
	close(fd); 

	// Set GPIO Direction to out
	fd=open("/sys/class/gpio/gpio18/direction", O_WRONLY); 
	if (fd<0) fprintf(stderr, "Error enabling direction\n"); // Error Checking
	write(fd,"out",3); 
	close(fd); 

	// Alternate the value of GPIO 18 between 1 and 0, blinking LED
	fd=open("/sys/class/gpio/gpio18/value", O_WRONLY);
	while (1){
		strcpy(buffer, "1"); 	
		write(fd,buffer,1);	// LED on
		usleep(500000); 	// Wait 0.5 seconds
		strcpy(buffer, "0");	 
		write(fd,buffer,1);	// LED off
		usleep(500000); 	// Wait 0.5 seconds
	}

	return 0;

}
