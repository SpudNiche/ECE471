// ECE 471 - HW 5
// Author: Nicholas LaJoie
// File: display_final.c 

#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>

#include <linux/i2c-dev.h>

// Letter Macros

#define wC 0x39
#define wE 0x79
#define w4 0x66
#define w7 0x07
#define w1 0x06

int main(int argc, char **argv) {

	int fd, result; // File descriptor and result flag 

	char i2c_device[]="/dev/i2c-1"; 

	unsigned char buffer[17];

	/* Open i2c device */

	fd = open(i2c_device, O_RDWR); 	// Open the device for read/write
	if(fd == -1) {
		printf("Error opening device\n");
		return 0; 
	}

	/* Set slave address */

	result = ioctl(fd, I2C_SLAVE, 0x70); 	// Set slave address 
	if(result < 0) {
		printf("Error setting slave address\n"); 
		return 0;
	}

	/* Turn on oscillator */

	buffer[0]=(0x2<<4)|(0x1);	// Initialize oscillator 
	result=write(fd, buffer, 1); 
	if(result < 0) {
                printf("Error turning on oscillator\n"); 
                return 0;
        }

	/* Turn on Display, No Blink */

	buffer[0]=(0x8<<4)|(0x1); 	// Set display on, no blink 
	result=write(fd, buffer, 1); 
	if(result < 0) {
                printf("Error turning on display\n"); 
                return 0;
        }

	/* Set Brightness */

	buffer[0]=(0xe<<4)|(0x9); 		// Set brightness to 10/16 DC
	result=write(fd, buffer, 1); 
	if(result < 0) {
                printf("Error setting brightness\n"); 
                return 0;
        }

	/* Write "ECE 471" to the screen */
	// Cell 1: buffer 1
	// Cell 2: buffer 3
	// Cell 3: buffer 7
	// Cell 4: buffer 9
	// Set buffer 0, 2, 4, 5, 6, 8 to 0x00 for write and no colon 
	
	int i; 
	char w_ece[16] = {0, wE, 0, wC, 0, 0, 0, wE, 0, 0, 0, 0, 0, 0, 0, 0}; // ‘ECE’
	char w_471[16] = {0, w4, 0, w7, 0, 0, 0, w1, 0, 0, 0, 0, 0, 0, 0, 0}; // ‘471’

	/* Clear Screen */
	for(i=0;i<16;i++) buffer[1+i]=0x00;
        result = write(fd, buffer, 17);
	if(result < 0) {
               	printf("Error clearing screen\n"); 
       	}

	while (1) {
		/* Write ECE */ 
		for(i=0;i<16;i++) buffer[i]=w_ece[i];
        	result = write(fd, buffer, 17);
		if(result < 0) {
                	printf("Error writing ECE\n"); 
        	}

		usleep(1000000); 	//Wait for 1 second

		/* Write 471 */
		for(i=0;i<16;i++) buffer[i]=w_471[i];
        	result = write(fd, buffer, 17);
		if(result < 0) {
                	printf("Error writing 471\n"); 
        	}

		usleep(1000000); 	//Wait for 1 second
	}

	/* Close device */

	close(fd); 

	return 0;
}
