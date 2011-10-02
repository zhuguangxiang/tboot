/*
 * uart.c
 *
 *  Created on: 2011-1-27
 *      Author: ForrestChu
 */
#include "s3c2440.h"
#include "uart.h"

void serial_init(void)
{
	/* use GPH port as uart0 */
	rGPHCON |= 0xa0;
	rGPHUP |= 0x0c;

	/* data bits=8,stop bit=1,no parity */
	rULCON0 = 0x03;
	/* PCLK for UART baud rate,interrupt request or polling mode */
	rUCON0 = 0x05;
	/* no FIFO */
	rUFCON0 = 0x07;
	/* no AFC */
	rUMCON0 = 0x00;
	/* UBRDIVn = (UART_clock / (buad_rate * 16)-1) */
	/* clock = pclk = 50Mhz, buadrate = 115200 */
	rUBRDIV0 = 26; //115200

	/* send using polling */
}

/*
 * Output a single byte to the serial port.
 */
void serial_putc(const char c)
{
	/* wait for room in the transmit FIFO */
	while(!(rUTRSTAT0 & 0x02));

	rUTXH0 = (unsigned char)c;
	/*
		to be polite with serial console add a line feed
		to the carriage return character
	*/
	if (c == '\n')
	{
		rUTXH0 = (unsigned char)'\r';
	}

}

void serial_puts(const char *s)
{
	while (*s)
	{
		serial_putc(*s++);
	}
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int serial_tstc(void)
{
	return (rUTRSTAT0 & 0x01);
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
char serial_getc(void)
{
	int rv = -1;
	while(1) 
	{
		rv = serial_tstc();
		
		if (rv > 0)
		{
			return rURXH0;
		}
	}
}

#define DELAY          1  /* ms */
void sys_timer_mdelay(int msec);
int serial_getc_timeout(char *ch, unsigned int timeout_ms)
{
	unsigned int counter = 0;

	while (serial_tstc() == 0)
	{
		if (counter > (timeout_ms / DELAY))
		{
			return 0;
		}
		sys_timer_mdelay(DELAY);
		counter++;
	}

	*ch = serial_getc();
	return 1;

}
