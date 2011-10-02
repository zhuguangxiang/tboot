#include "command.h"
#include "uart.h"
#include "stddef.h"
#include "xmodem.h"
#include "string.h"

extern void sys_timer_mdelay(unsigned int msec);
extern int serial_getc_timeout(char *ch, unsigned int timeout);
extern void printk(const char *fmt, ...);
extern long strtol(const char *cp, char **endp, unsigned int base);

struct xmodem_client xmodem_1k;

char *xmodem_buf = (char *)0x30100000;
#define XMDM_BUF_SIZE   1024*1024

int do_xmodem(struct cmd_table *cmdtp, int argc, char *argv[])
{	
    int state = 0;
    int size = 0, total_size = 0;
    bool eof = false;
    bool crc_mode = false;
    int address = 0;
    char ch_tmp = 0;
    
    if (argc < 2)
    {
        printk("%s\n", cmdtp->usage);
        return -1;
    }

    address = strtol(argv[1], NULL, 0);
    
    if (argc > 2)
    {
        if (strncmp(argv[2], "true", strlen(argv[2])) == 0)
        {
            crc_mode = true;
        }
    }
    
    memset((void*)address, 0, XMDM_BUF_SIZE);
    
    printk("using %s check\n", (crc_mode == true) ? "crc":"calcsum");
 
    printk("starting xmodem transfer after 30s, Press Ctrl+C to cancel.\n");

    serial_getc_timeout(&ch_tmp, 30*1000);
    
    if (ch_tmp == 3)
    {
        return -1;
    }
  
    xmdm_client_init(&xmodem_1k, crc_mode, serial_putc, serial_getc_timeout);
    xmdm_client_open(&xmodem_1k);

    while (eof != true)
    {
        size = 0;
        state = xmdm_client_read(&xmodem_1k, (char *)address+total_size, &size, &eof);
        total_size += size;
        if (state != 0)
        {
            printk("Transferring error :%d, size:%d\n", state, size);
            return 0;
        }
    }
  
    printk("Transferring finished:%u\n", total_size);
    return 0;
} 

TBOOT_CMD(xmdm, 3, do_xmodem, "usgae:xmdm addr [true/fasle]", 0);

typedef void (*IMAGE_ENTRY)(void);

int do_go(struct cmd_table *cmdtp, int argc, char *argv[])
{
    int address = 0;
    IMAGE_ENTRY entry_func = NULL;
    if (argc < 2)
    {
        printk("%s\n", cmdtp->usage);
        return -1;
    }
    
    address = strtol(argv[1], NULL, 0);
    entry_func = (IMAGE_ENTRY)address;
    entry_func();
    return 0;
}

TBOOT_CMD(go, 2, do_go, "execute binary image: go address", 0);


