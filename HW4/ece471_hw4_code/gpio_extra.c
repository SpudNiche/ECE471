// ECE 471 - HW 4
// Author: Nicholas LaJoie
// Fild: gpio_extra.c

#include <stdio.h>

#include <fcntl.h>	/* open() */
#include <unistd.h>	/* close() */
#include <string.h>

// Something cool: Light up an LED every time the button is pressed 

int main(int argc, char **argv) {

	char buffer[10];
        int fd;
 
	// Initialize GPIO Pins
        fd=open("/sys/class/gpio/export", O_WRONLY); 
        strcpy(buffer,"18"); 
        write(fd,buffer,2);
	close(fd); 

	fd=open("/sys/class/gpio/export", O_WRONLY);
	strcpy(buffer,"17");  
        write (fd, buffer, 2); 
        close (fd);

	// Set GPIO 18 to output, 17 to input 
	fd=open("/sys/class/gpio/gpio18/direction", O_WRONLY); 
        write(fd,"out",3); 
        close(fd);

	fd=open("/sys/class/gpio/gpio17/direction", O_WRONLY); 
        write(fd,"in",2);
        close(fd);

	// Set pin 18 to 0, get initial read of pin 17
	fd=open("/sys/class/gpio/gpio18/value", O_WRONLY);
	strcpy(buffer, "0");
	write(fd,buffer,1); // GPIO 18 low
	close(fd); 

	fd=open("/sys/class/gpio/gpio17/value", O_RDONLY);
        read(fd,buffer,16); // Read GPIO 17
        close(fd);

	while (1) {
                while (buffer[0] == '0') {
                        fd = open ("/sys/class/gpio/gpio17/value", O_RDONLY);
                        read (fd, buffer, 16);
                        close (fd);
                }
		// LED on! 
                fd=open("/sys/class/gpio/gpio18/value", O_WRONLY);
        	strcpy(buffer, "1");
        	write(fd,buffer,1); // GPIO 18 high
        	close(fd); 

                while (buffer[0] == '1') {
                        fd = open ("/sys/class/gpio/gpio17/value", O_RDONLY);
                        read (fd, buffer, 16);
                        close (fd);
                }
		// LED off! 
		fd=open("/sys/class/gpio/gpio18/value", O_WRONLY);
       		strcpy(buffer, "0");
        	write(fd,buffer,1); // GPIO 18 low
        	close(fd); 
        }

	return 0;

}
