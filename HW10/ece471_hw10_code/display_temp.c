// Author: Nicholas LaJoie
// File: display_temp.c

/* Description */
// Display the temperature in Farenheit from a one-wire temp sensor on the i2c display

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define TEMP_SENSOR "/sys/bus/w1/devices/28-0316532959ff/w1_slave" // Temp Device

void display(double temp); 								// Function to display to i2c
static double read_temp(char *filename); 	// Function to read the temperature

int main(int argc, char **argv) {

	/* Variables */
	double tempF;

	/* Continuously print temperature value in farenheit every second */
	while(1) {
		tempF=read_temp(TEMP_SENSOR);   			// Grab temperature
		printf("Temp: %.2lfF\n",tempF);				// Print to console
	 	display(tempF); 											// Display to i2c screen
		usleep(1000000);											// Wait 1 second
	}

	return 0;
}

static double read_temp(char *filename) {

	/* Variable Declarations */
	double result=0.0; // Result value
	static char  string1[256], string2[256]; 	 // Result as a string
	FILE *fff;

	/* Read file Using fscanf */
	fff=fopen(filename, "r");			 // Open the file for reading
	if(fff == NULL){ 							 // Error checking
		printf("Can't open file one-wire file!\n");
		exit(-1); // Exit
	}

	/* Modified my approach from the last time -- simplifies it greatly */
	// Scans all items, '*' indicates the string item is ignored

	fscanf(fff,"%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %s %*s %*s %*s %*s %*s %*s %*s %*s %*s %s", string1, string2);

	/* Compare String1 to "YES" */
	if(strcmp(string1, "YES") != 0){
		printf("Error: Not \"YES\" value!\n");
		result = -1000; // Return invalid temp
	}

	else {
		/* Grab temperature value */
		result = atof(string2 + 2); 		// '+ 2' removes 't=', converts string to float
		result = result/1000.00;  			// Convert from milli-celsius to celsius
		result = result*(9.0/5.0) + 32; // Convert to farenheit
	}

	return result;
}


void display(double temp) {

	// Cell 1: buffer 1
	// Cell 2: buffer 3
	// Colon: buffer 5
	// Cell 3: buffer 7
	// Cell 4: buffer 9
	// Set buffer 0, 2, 4, 6, 8 to 0x00 for write

	/* Variable declarations */
	char print_num[10]={0x3F, 0x06, 0x5B, 0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67}; 	// Array that maps digits 0-9 on display
	char degree = 0x63;
	char negative =0x40;
	char r_string[5]; // Array to hold displayable version of the temp.

	/* Initialize */
	int fd, result, i; // File descriptor and result flag
	char i2c_device[]="/dev/i2c-1";
	unsigned char buffer[17];

	/* Open i2c device */
	fd = open(i2c_device, O_RDWR); 	// Open the device for read/write
	if(fd == -1) {
		printf("Error opening device\n");
		exit(-1);
	}

	/* Set slave address */
	result = ioctl(fd, I2C_SLAVE, 0x70); 	// Set slave address
	if(result < 0) {
		printf("Error setting slave address\n");
		exit(-1);
	}

	/* Turn on oscillator */
	buffer[0]=(0x2<<4)|(0x1);	// Initialize oscillator
	result=write(fd, buffer, 1);
	if(result < 0) {
		printf("Error turning on oscillator\n");
		exit(-1);
	}

	/* Turn on Display, No Blink */
	buffer[0]=(0x8<<4)|(0x1); 	// Set display on, no blink
	result=write(fd, buffer, 1);
	if(result < 0) {
		printf("Error turning on display\n");
		exit(-1);
	}

	/* Set Brightness */
	buffer[0]=(0xe<<4)|(0x9); 		// Set brightness to 10/16 DC
	result=write(fd, buffer, 1);
	if(result < 0) {
		printf("Error setting brightness\n");
		exit(-1);
	}

	/* Clear Screen */
	for(i=0;i<16;i++) buffer[1+i]=0x00;
	result = write(fd, buffer, 17);
	if(result < 0) {
		printf("Error clearing screen\n");
		exit(-1);
	}

	/*************************************************/
	/* Convert from double to displayable characters */
	/*************************************************/

	/* Convert to string */
	snprintf(r_string, 5, "%lf", temp); // Stores the first 5 digits/char of the temp in a string

	/********************************************/
	/* Print temperature */
	/********************************************/

	/* If temp is between 0 and 99.9, inclusive */
	/* Special case for when temp is a single digit is implemented */
	if (temp >= 0.0 && temp <=99.9) {
		// First digit
		buffer[0]=0x00;
		if (temp < 10.0) buffer[1]=0x00;
		else buffer[1]=print_num[r_string[0] - '0'];

		// Second digit with decimal point
		buffer[2]=0x00;
		if (temp < 10.0) buffer[3]=0x80|print_num[r_string[0] - '0'];
		else buffer[3]=0x80|print_num[r_string[1] - '0'];

		// Third digit
		buffer[6]=0x00;
		if (temp < 10.0) buffer[7]=print_num[r_string[2] - '0'];
		else buffer[7]=print_num[r_string[3] - '0'];

		// Degree symbol
		buffer[8]=0x00;
		buffer[9]=degree;
	}

	/* If temp is between -99 and 0 */
	/* Special case for when temp is a single digit is implemented */
	else if (temp >= -99.9 && temp < 0.0) {
		// Negative Sign
		buffer[0]=0x00;
		buffer[1]=negative;

		// Second digit
		buffer[2]=0x00;
		if (temp > -10.0) buffer[3]=0x00;
		else buffer[3]=print_num[r_string[1] - '0'];

		// Third digit
		buffer[6]=0x00;
		if (temp > -10.0) buffer[7]=print_num[r_string[1] - '0'];
		else buffer[7]=print_num[r_string[2] - '0'];

		// Degree symbol
		buffer[8]=0x00;
		buffer[9]=degree;
	}

	/* If temp is between 100 and 999, inclusive */
	else if (temp >= 100.0 && temp <=999.9) {
		// First digit
		buffer[0]=0x00;
		buffer[1]=print_num[r_string[0] - '0'];

		// Second digit
		buffer[2]=0x00;
		buffer[3]=print_num[r_string[1] - '0'];

		// Third digit
		buffer[6]=0x00;
		buffer[7]=print_num[r_string[2] - '0'];

		// Degree symbol
		buffer[8]=0x00;
		buffer[9]=degree;
	}

	/* Else, display invalid pattern, '-- --' */
	else {
		buffer[0]=0x00;
		buffer[1]=negative;
		buffer[2]=0x00;
		buffer[3]=negative;
		buffer[6]=0x00;
		buffer[7]=negative;
		buffer[8]=0x00;
		buffer[9]=negative;
	}

	/* Write to screen */ 
	result = write(fd, buffer, 17);
	if(result < 0) {
		printf("Error writing temp.\n");
		exit(-1);
	}

	/* Close device */
	close(fd);

}
