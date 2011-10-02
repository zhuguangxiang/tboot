/*
 * stdarg.h
 *
 *  Created on: 2009-8-10
 *      Author: ForrestChu
 */
#ifndef _STDARG_H
#define _STDARG_H

typedef char *va_list;

/* 
 *
 * __va_size是一个为了考虑内存对齐而设计的.
 * 当一个变量占用1-4个字节的时候,在内存都占用4个字节,这就是4字节对齐原则,“加宽”原则.
 * 当变量占用5-8个字节的时候,在内存中全部占用8个字节.
 *
 */

#define __va_size(TYPE)		( (sizeof(TYPE) + sizeof (int) - 1) & ~(sizeof (int) - 1) )

/*
 * 这个宏，初始化了变参,AP指向第一个变参,AP的类型为va_list
 * FIRSTARG是第一个显式参数,不是变参哦!~
 */
#define va_start(AP, FIRSTARG)	( AP = ( (va_list)&FIRSTARG + __va_size(FIRSTARG) ) )

/*
 * 取参数列表中的值,按顺序取.取出的值是什么类型的,由用户字节去解析，本宏无法解析:-)
 */
#define va_arg(AP, TYPE)	( *(TYPE *)( ( AP += __va_size(TYPE) ) - __va_size(TYPE) ) )				\

/*
 * 将AP清零,退出变参处理.
 */
#define va_end(AP)	( AP = (va_list)0 )

#endif /* _STDARG_H */
