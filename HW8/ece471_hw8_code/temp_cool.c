// Author: Nicholas LaJoie
// File: temp_cool.c
// Something cool: Various temperature units

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define SENSOR_NAME "/sys/bus/w1/devices/28-0316532959ff/w1_slave" // Temp Device

static double read_temp(char *filename) {

	/* Variable Declarations */
	double result=0.0;
	int milli_value,i,j=0;
	int size_1;
	char  string1[256], string2[256], temp[256], milli[256];
	FILE *fff;

	/* Read File Using fscanf */
	fff=fopen(filename, "r");			 // Open the file for reading
	fscanf(fff,"%[^\n]\n %[^\n]\n",string1,string2); // Scan the first two lines

	/* Determine if first line ends with "YES" */
	size_1 = strlen(string1); // Get string length
	i=size_1;
	while(string1[i] != ' ') i--; // Trace back to first blank space

	/* Place end of line 1 in a temp string to compare */
	i++; // Increment to first character
	while(string1[i] != '\0'){
		temp[j]=string1[i];
		i++;
		j++;
	}
	temp[j] = '\0';

	/* Compare temp to "YES" */
	if(strcmp(temp, "YES") != 0){
		printf("Error: Not \"YES\" value!\n");
		return -99; // Return invalid temp.
	}

	/* Grab temperature from the end of second line */
	i=0;
	j=0;

	/* Move to t=XXXX */
	while(string2[i] != '=') i++;
	i++;

	/* Place end of string 2 into milli string - the temperature value */
	while(string2[i] != '\0'){
		milli[j] = string2[i];
		i++;
		j++;
	}
	milli[j] = '\0';
	milli_value = atoi(milli); // Convert string to integer value
	result = milli_value / 1000.0; // Convert from milli-C to C

	return result;
}

int main(int argc, char **argv) {

	double tempC, tempF, tempK; // Temperature variables

	/* Continuously print temperature value in various units every second */
	while(1) {

		tempC=read_temp(SENSOR_NAME); // Grab temp
		tempF=tempC*(9.0/5.0) + 32;   // Convert to F
		tempK=tempC + 273.15;	      // Convert to K
		printf("%.2lfC %.2lfF %.2lfK\n",tempC,tempF,tempK);
		sleep(1);
	}

	return 0;
}
