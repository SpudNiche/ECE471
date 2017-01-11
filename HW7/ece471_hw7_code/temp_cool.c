// Author: Nicholas LaJoie
// File: temp_cool.c
// Something Cool: Display high and low every minute

#include <stdio.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#define LENGTH 3

int main(int argc, char **argv) {

	/* Variable Declarations */
        int spi_fd, count;
        double ch2_value, deg_c, deg_f, low_c, low_f, high_c, high_f;
        int result;
	struct spi_ioc_transfer spi;
        unsigned char data_in[LENGTH], data_out[LENGTH];

	/* For Full Duplex support - taken from course notes */
	/* kernel doesn’t like it if stray values, even in padding */
        memset (&spi, 0, sizeof(struct spi_ioc_transfer));

        /* Setup full-duplex transfer of 3 bytes */
        spi.tx_buf = (unsigned long)&data_out;
        spi.rx_buf = (unsigned long)&data_in;
        spi.len = LENGTH;
        spi.delay_usecs = 0;
        spi.speed_hz = 100000;
        spi.bits_per_word = 8;
        spi.cs_change = 0;

	/* Open SPI device */
	spi_fd = open("/dev/spidev0.0", O_RDWR);
	if(spi_fd == 0) {
		printf("Error opening SPI device!\n"); 
	}

	/* Set SPI Mode_0 */
	int mode = SPI_MODE_0;
	result = ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
	if(result < 0) {
		printf("Error setting SPI Mode!\n"); 
	}

	/* Set 8 bits per word */
	int bpw_mode = spi.bits_per_word; 
	result = ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &bpw_mode);
	if(result < 0) printf("Error setting bpw - read!\n");
	result = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bpw_mode);
	if(result < 0) printf("Error setting bpw - write!\n");

	/* Set 100 kHz max frequency */
	int mfq_mode = spi.speed_hz; // Set frequency to 100kHz
	result = ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &mfq_mode);
	if(result < 0) printf("Error setting max freq. - read!\n");
	result = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &mfq_mode);
	if(result < 0) printf("Error setting max freq. - write!\n");

	/* Loop forever printing the CH2 Voltage 		*/
	/* Once per second.					*/
	count = 0; // Initialize counter variable
	while(1) {

		/* Read Channel 2 - hooked up to temp. sensor */

		/* Set up data out */
		data_out[0] = 0x01; // Start bit
		data_out[1] = 0xA0; // Single ended mode, Channel 2, Zeros
		data_out[2] = 0x00; // Zeros

		/* Run one full-duplex transaction - from course notes */
	        result = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi);
		if(result < 0 ) printf("Error during spi transaction!\n");

		/* Receive 3 Bytes - first byte ignored */
		ch2_value = ((data_in[1]<<8) + data_in[2])&0x03FF; // Preserve bits 9-0 only

		/* Convert 10 bits to fp voltage */
		ch2_value = (ch2_value * 3.3)/1024;

		/* Convert to degrees C & degress F */
		deg_c = (100 * ch2_value) - 50;
		deg_f = (deg_c * (9.0/5.0)) + 32;

		/* If first pass of the loop, initialize new highs and lows */
		if(count == 0){
			low_c = deg_c;
			high_c = deg_c;
			low_f = deg_f;
			high_f = deg_f;
		}

		/* Set lows */
		if(deg_c < low_c) low_c = deg_c;
		if(deg_f < low_f) low_f = deg_f;

		/* Set highs */
		if(deg_c > high_c) high_c = deg_c;
		if(deg_f > high_f) high_f = deg_f;

		/* Print Results with 2 decimal places */
		printf("Celsius: %.2lf degrees\nFarenheit: %.2lf degrees\n\n", deg_c, deg_f);
		count++;

		/* Every minute, print highs and lows for the minute */
		if(count == 60) {
			printf("For the minute:\n");
			printf("High (C): %.2lf degrees, High (F): %.2lf degrees\n", high_c, high_f);
			printf("Low (C): %.2lf degrees, Low (F): %.2lf degrees\n\n", low_c, low_f);
			count = 0;
		}

		/* Delay one second */
		usleep(1000000);
	}

	return 0;
}
