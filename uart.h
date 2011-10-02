/*
 * uart.h
 *
 *  Created on: 2011-1-27
 *      Author: ForrestChu
 */

#ifndef UART_H_
#define UART_H_
#include "s3c2440.h"

void serial_init(void);
void serial_putc(const char c);
void serial_puts(const char *s);
int serial_tstc(void);
char serial_getc(void);
int serial_getc_timeout(char *ch, unsigned int timeout_ms);
#endif /* UART_H_ */
