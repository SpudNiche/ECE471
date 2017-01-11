// ECE 471 - HW 5
// Author: Nicholas LaJoie
// File: display_test.c

#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>

#include <linux/i2c-dev.h>

int main(int argc, char **argv) {

	int fd, result;

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

	buffer[0]=(0x2<<4)|(0x1);
	result=write(fd, buffer, 1);
	if(result < 0) {
                printf("Error turning on oscillator\n"); 
                return 0;
        }

	/* Turn on Display, No Blink */

	buffer[0]=(0x8<<4)|(0x1);
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
	/* Write 1s to all Display Columns */

	int i;
	buffer[0]=0x0;
	for(i=0;i<16;i++) buffer[1+i]=0xff;
	result=write(fd, buffer, 17);
	if(result < 0) {
                printf("Error writing to display\n"); 
                return 0;
        }

	/* Close device */

	close(fd);

	return 0;
}
