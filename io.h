/*
 * io.h
 *
 *  Created on: 2011-1-27
 *      Author: ForrestChu
 */

#ifndef IO_H_
#define IO_H_

#define outb(reg,val)   *(volatile unsigned char *)(reg) = (val)
#define inb(reg)        *(volatile unsigned char *)(reg)

#define outw(reg,val)   *(volatile unsigned short *)(reg) = (val)
#define inw(reg)        *(volatile unsigned short *)(reg)

#define outl(reg,val)   *(volatile unsigned int *)(reg) = (val)
#define inl(reg)        *(volatile unsigned int *)(reg)

#endif /* IO_H_ */
