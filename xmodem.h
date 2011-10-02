#ifndef _XMODEM_H_
#define _XMODEM_H_
#include "stddef.h"

#define xyzModem_access   -1
#define xyzModem_noZmodem -2
#define xyzModem_timeout  -3
#define xyzModem_eof      -4
#define xyzModem_cancel   -5
#define xyzModem_frame    -6
#define xyzModem_cksum    -7
#define xyzModem_sequence -8
#define xyzModem_retries  -9

#define xyzModem_CHAR_TIMEOUT            2000	/* 2 seconds */
#define xyzModem_MAX_RETRIES             10
#define xyzModem_MAX_RETRIES_WITH_CRC    10
#define xyzModem_CAN_COUNT                3	/* Wait for 3 CAN before quitting */


/* Values magic to the protocol */
#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06

#define NAK 0x15
#define CAN 0x18
#define EOF 0x1A		/* ^Z for DOS officionados */

#define false  0
#define true   1

typedef struct xmodem_client XMDM_CLIENT;

typedef int (*XMDM_RECV_CHAR_TMO_FUNC)(char *ch, unsigned int tmo_ms);
typedef void (*XMDM_SEND_CHAR_FUNC)(const char ch);

#define XMDM_CLIENT_BUF_SIZE    1024

struct xmodem_client
{
    unsigned char pkt[XMDM_CLIENT_BUF_SIZE];
    
    unsigned short pkt_len;
    char pad0[2];
    
    unsigned char blk, cblk, crc1, crc2;
    
    unsigned char next_blk;	/* Expected block */
    char pad1[3];
    
    bool using_crc;
    
    XMDM_SEND_CHAR_FUNC send_char_func;
    XMDM_RECV_CHAR_TMO_FUNC recv_char_tmo_func;
};

void xmdm_client_init(XMDM_CLIENT *xmdm, bool using_crc, 
                       XMDM_SEND_CHAR_FUNC uart_send_char,
                       XMDM_RECV_CHAR_TMO_FUNC uart_recv_char_tmo);

int xmdm_client_open(XMDM_CLIENT *xmdm);

int xmdm_client_recv_pkt(XMDM_CLIENT *xmdm);

int xmdm_client_read(XMDM_CLIENT *xmdm, char *buf, int *size, bool *eof);

#endif

