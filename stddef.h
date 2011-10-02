/*
 * stddef.h
 *
 *  Created on: 2009-8-10
 *      Author: ForrestChu
 */
#ifndef _STDDEF_H
#define _STDDEF_H

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef char	s8_t;
typedef short	s16_t;
typedef int		s32_t;

/* lock_t must be volatile for optimization by gcc */
typedef volatile unsigned int lock_t;


#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#ifndef NAME
#define NAME(x)  #x
#endif

typedef volatile unsigned int *regp_t;
typedef volatile unsigned int reg_t;
typedef volatile unsigned long long reg64_t;

#ifndef LOG_IT
#define LOG_IT
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#define true	1
#define false	0

typedef int  bool;

#define cli() do{	asm volatile("mrs ip, cpsr\n\t"	 		\
								"orr ip, ip, #0x80\n\t" 	\
								"msr cpsr, ip"				\
								:		\
								:		\
								:"ip", "cc");}while(0)

#define sti() do{	asm volatile("mrs ip, cpsr\n\t"			\
								 "bic ip, ip, #0x80\n\t"	\
								 "msr cpsr, ip"				\
								 :		\
								 :		\
								 :"ip", "cc");}while(0)
#endif
