// Author: Nicholas LaJoie
// File: test_spi.c

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
        int spi_fd;
        double ch0_value, ch1_value;
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

	/* Loop forever printing the CH0 and CH1 Voltages 	*/
	/* Once per second.					*/
	while(1) {

		/* Read Channel 0 */

		/* Set up data out */
		data_out[0] = 0x01; // Start bit
		data_out[1] = 0x80; // Single ended mode, Channel 0, Zeros
		data_out[2] = 0x00; // Zeros

		/* Run one full-duplex transaction - from course notes */
	        result = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi);

		/* Receive 3 Bytes - first one byte ignored */
		ch0_value = ((data_in[1]<<8) + data_in[2])&0x03FF; // Preserve bits 9-0 only

		/* Read Channel 1 */

                /* Set up data out */
                data_out[0] = 0x01; // Start bit
                data_out[1] = 0x90; // Single ended mode, Channel 1, Zeros
                data_out[2] = 0x00; // Zeros

                /* Run one full-duplex transaction - from course notes */
                result = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi);

                /* Receive 3 Bytes - first one byte ignored */
                ch1_value = ((data_in[1]<<8) + data_in[2])&0x03FF; // Preserve $

		/* Get 10 bits as integer, convert to fp voltage */
		ch0_value = (ch0_value * 3.3)/1024; // Floating point voltage
		ch1_value = (ch1_value * 3.3)/1024;

		/* Print Results - Two decimal places */
		printf("Channel 0 Value: %.2lfV\nChannel 1 Value: %.2lfV\n\n", ch0_value, ch1_value);

		/* Delay one second */
		usleep(1000000);
	}

	return 0;
}
