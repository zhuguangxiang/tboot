#ifndef _CRC_H_
#define _CRC_H_

/* 16 bit CRC with polynomial x^16+x^12+x^5+1 */
extern unsigned short cyg_crc16(unsigned char *s, int len);

#endif /* _CRC_H_ */
