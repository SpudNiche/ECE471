// ECE 471 - HW 5
// Author: Nicholas LaJoie
// File: display_cool.c

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>

#include <linux/i2c-dev.h>

int main(int argc, char **argv) {

	time_t rawtime; 	// Plain time that returns from time function 
	struct tm *info; 	// Struct for time in components (hrs, min, etc.)

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
	// Set buffer 0, 2, 4, 6, 8 to 0x00 for write

	int i;
	
	// Array that maps digits 0-9 on display 
	char print_num[10]={0x3F, 0x06, 0x5B, 0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67}; 

	/* Clear Screen */
	for(i=0;i<16;i++) buffer[1+i]=0x00;
       	result = write(fd, buffer, 17);
	if(result < 0) {
                printf("Error clearing screen\n"); 
        }

	while(1){
		/* Get time */
		time(&rawtime);
		info = localtime(&rawtime);

		/* Print Colon */ 
		buffer[4] = 0x00;
		buffer[5] = 0x02;

		/* Print Hour-Tens */ 
		buffer[0] = 0x00;
		buffer[1] = print_num[(info->tm_hour)/10];

		/* Print Hour-Ones */
		buffer[2] = 0x00;
		buffer[3] = print_num[(info->tm_hour)%10];

		/* Print Minute-Tens */ 
		buffer[6] = 0x00;
		buffer[7] = print_num[(info->tm_min)/10];

		/* Print Minute-Ones */ 
		buffer[8] = 0x00;
		buffer[9] = print_num[(info->tm_min)%10];

		/* Write to screen */ 
		result = write(fd, buffer, 17);
		if(result < 0) {
                	printf("Error writing time\n"); 
        	}
		
		usleep(999999); // Limit update to just under a second
		// printf("%s", asctime(info)); // Used to verify time being used 
	}

	/* Close device */

	close(fd); 

	return 0;
}
