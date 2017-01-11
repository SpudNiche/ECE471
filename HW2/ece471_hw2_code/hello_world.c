// ECE471 HW 2
// Author: Nicholas LaJoie

#include <stdio.h>

int main(int argc, char **argv) {

	int v = 1; // Line number

	for (v = 1; v < 21; v++)
		{
		printf("\x1b[%dm #%d:ECE471 Carpe Diem!\n", 31+(v%6), v); // Loop custom message 20 times, use line number to change text color
									  // Color values range from 31 to 36
		}

	return 0;
}
