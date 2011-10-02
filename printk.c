#include "uart.h"
#include "stdarg.h"

extern int vsprintf(char * buf, const char * fmt, va_list args);

void printk(const char *fmt, ...)
{
	char buf[512];
	va_list args;
	unsigned int cnt;
	va_start(args, fmt);
	cnt = vsprintf(buf, fmt, args);
	va_end(args);
	buf[cnt] = 0;
	serial_puts(buf);
}
