/*
 * SST39VF1601 DEVICE DRIVER
 *
 */
#include "stddef.h"
#include "stdio.h"
#include "command.h"
#include "uart.h"

#include "sst39vf1601.h"

extern long strtol(const char *cp, char **endp, unsigned int base);
extern void sys_timer_mdelay(int msec);

NOR_FLASH NorFlash = {NOR_FLASH_ADDRESS, NORFLASH_MAN_ID, NORFLASH_DEV_ID,
                       NORFLASH_FLASH_SIZE, NORFLASH_SECTOR_SIZE, NORFLASH_BLOCK_SIZE};

void norflash_write_data(NOR_FLASH *flash, unsigned int address, unsigned short data)
{
    volatile unsigned short *data_address = NULL;
    
    data_address = (unsigned short *)(flash->base_address + (address << 1));
    *data_address = data;
}

unsigned short norflash_read_data(NOR_FLASH *flash, unsigned int address)
{
    volatile unsigned short *data_address = NULL;
    
    data_address = (unsigned short *)(flash->base_address + (address << 1));
    return *data_address;
}

void norflash_toggle(NOR_FLASH *flash)
{
	volatile unsigned short *toggle_address = (unsigned short *)(flash->base_address + 0x00);
    
	volatile unsigned short old = 0, new = 0;
    
	old = *toggle_address;
    
	while (1) 
    {
		new = *toggle_address;
        
		if ((old & 0x40) == (new & 0x40))  /* DQ6 is toggling... */
        {
			break;
		}
		else
		{
			old = new;
		}
	}
}

void norflash_word_prog(NOR_FLASH *flash, unsigned int address, unsigned short data)
{

    if (address >= flash->flash_size)
    {
        printk("ERROR:address(0x%x) is out of flash size, program abort\n", address);
        return;
    }

    address = flash->base_address + address;
    
    /* check whether address is not programmed */
    if (*(unsigned short *)address != 0xffff)
    {
        printk("ERROR:address(0x%x) has data(0x%x), program abort\n", 
                address, *(unsigned short *)address);
        return;
    }
    
    norflash_write_data(flash, SST39VF16001_PORT_1, 0xaa);
    norflash_write_data(flash, SST39VF16001_PORT_2, 0x55);
    norflash_write_data(flash, SST39VF16001_PORT_1, 0xa0);
    norflash_write_data(flash, address >> 1, data);
    
    norflash_toggle(flash);
}

void norflash_erase_sector(NOR_FLASH *flash, unsigned int sector_id)
{
    unsigned int sector_address = sector_id * flash->sector_size + flash->base_address;

    norflash_write_data(flash, SST39VF16001_PORT_1, 0xaa);
    norflash_write_data(flash, SST39VF16001_PORT_2, 0x55);
    norflash_write_data(flash, SST39VF16001_PORT_1, 0x80);
    
    norflash_write_data(flash, SST39VF16001_PORT_1, 0xaa);
    norflash_write_data(flash, SST39VF16001_PORT_2, 0x55);
    norflash_write_data(flash, sector_address >> 1, 0x30);

    norflash_toggle(flash);
}

void norflash_erase_block(NOR_FLASH *flash, unsigned int block_id)
{
    unsigned int block_address = block_id * flash->block_size + flash->base_address;

    norflash_write_data(flash, SST39VF16001_PORT_1, 0xaa);
    norflash_write_data(flash, SST39VF16001_PORT_2, 0x55);
    norflash_write_data(flash, SST39VF16001_PORT_1, 0x80);
    
    norflash_write_data(flash, SST39VF16001_PORT_1, 0xaa);
    norflash_write_data(flash, SST39VF16001_PORT_2, 0x55);
    norflash_write_data(flash, block_address >> 1, 0x50);

    norflash_toggle(flash);
}

void norflash_erase_chip(NOR_FLASH *flash)
{
    norflash_write_data(flash, SST39VF16001_PORT_1, 0xaa);
    norflash_write_data(flash, SST39VF16001_PORT_2, 0x55);
    norflash_write_data(flash, SST39VF16001_PORT_1, 0x80);
    
    norflash_write_data(flash, SST39VF16001_PORT_1, 0xaa);
    norflash_write_data(flash, SST39VF16001_PORT_2, 0x55);
    norflash_write_data(flash, SST39VF16001_PORT_1, 0x10);

    norflash_toggle(flash);
}

bool norflash_sector_blank(NOR_FLASH *flash, unsigned int sector_id)
{
    bool blank = true;
    unsigned int sector_address = sector_id * flash->sector_size + flash->base_address;
    unsigned int idx = 0;
    unsigned short *pdata = (unsigned short *)sector_address;
    
    while (idx < flash->sector_size/2)
    {
        if (*(pdata + idx) != 0xffff)
        {
            blank = false;
            break;
        }
        else
        {
            idx++;
        }
    }

    return blank;
}

void norflash_read_id(NOR_FLASH *flash, unsigned short *man_id, unsigned short *dev_id)
{
    /* software ID Entry */
    norflash_write_data(flash, SST39VF16001_PORT_1, 0xaa);
    norflash_write_data(flash, SST39VF16001_PORT_2, 0x55);
    norflash_write_data(flash, SST39VF16001_PORT_1, 0x90);

    sys_timer_mdelay(100);

    /* software ID Ready */
    *man_id = norflash_read_data(flash, 0x00);      /* A0 == 0 */
    *dev_id = norflash_read_data(flash, 0x01);      /* A0 == 1 */

    /* software ID Exit */
    norflash_write_data(flash, SST39VF16001_PORT_1, 0xaa);
    norflash_write_data(flash, SST39VF16001_PORT_2, 0x55);
    norflash_write_data(flash, SST39VF16001_PORT_1, 0xf0);
}

void norflash_write_buf(NOR_FLASH *flash, unsigned int flash_addr, 
                        unsigned int buf_addr, unsigned int buf_size)
{
    unsigned int idx = 0;
    unsigned int erase_sector_id = 0, erase_sector_num = 0;

    flash_addr = flash_addr & ~3;
    
    erase_sector_id = flash_addr / flash->sector_size;
    
    buf_size = (buf_size + 3) & ~3;
    
    erase_sector_num = buf_size / flash->sector_size + 1;

    printk("%d, %d\n", erase_sector_id, erase_sector_num);
    while (erase_sector_num > 0)
    {
        norflash_erase_sector(flash, erase_sector_id);
        erase_sector_id++;
        erase_sector_num--;
    }

    sys_timer_mdelay(5000);

    printk("%x, %x, %d\n", flash_addr, buf_addr, buf_size);
    for (idx = 0; idx < buf_size; idx += 2)
    {
        norflash_word_prog(flash, flash_addr + idx, *(unsigned short *)(buf_addr + idx));
    }
}

#define NORFLASH_CMD_HELP           0
#define NORFLASH_CMD_INFO           1
#define NORFLASH_CMD_ERASE_SECTOR   2
#define NORFLASH_CMD_ERASE_BLOCK    3
#define NORFLASH_CMD_WRITE          4

#define CONFIG_NORFLASH_MODE    "NORFLASH-TBoot>"

void norflash_menu(void)
{
    printk("-------------NORFLASH-MENU---------------\n");
    printk("norflash 0 ---- show this menu\n");
    printk("norflash 1 ---- show flash information\n");
    printk("norflash 2 <sector-id> ---- erase 'sector-id' sector\n");
    printk("norflash 3 <block-id> ---- erase 'block-id' block\n");
    printk("norflash 4 <flash addr> <mem addr> <mem size> ---- write mem data to flash\n");
    printk("-----------------------------------------\n");
}

void norflash_info(void)
{
    unsigned short man_id = 0, dev_id = 0;
    
    norflash_read_id(&NorFlash, &man_id, &dev_id);
    
	printk("-------------NORFLASH(Size: 0x%08x)-------------\n", NorFlash.flash_size);
    printk("Base Address:0x%08x\n", NorFlash.base_address);
    printk("Man-ID:0x%04x, Dev-ID:0x%04x\n", man_id, dev_id);
    printk("Sector Size:0x%08x, Block Size:0x%08x\n", NorFlash.sector_size, NorFlash.block_size);
    printk("----------------------------------------------------\n");
}

int do_norflash(struct cmd_table *cmdtp, int argc, char *argv[])
{
    int cmd = 0, sector_id = 0, block_id = 0;
    unsigned int flash_addr = 0, mem_addr = 0, mem_size = 0;
    
    if (argc < 2)
    {
        argv[1] = "0";
    }
    
    cmd = strtol(argv[1], NULL, 0);

    switch (cmd)
    {
        case NORFLASH_CMD_HELP:
        {
            norflash_menu();
        }
        break;
        
        case NORFLASH_CMD_INFO:
        {
            norflash_info();
        }
        break;
        
        case NORFLASH_CMD_ERASE_SECTOR:
        {
            if (argc < 3)
            {
                printk("%s\n", cmdtp->usage);
                return -1;
            }
            sector_id = strtol(argv[2], NULL, 0);
        }
        break;
        
        case NORFLASH_CMD_ERASE_BLOCK:
        {
            if (argc < 3)
            {
                printk("%s\n", cmdtp->usage);
                return -1;
            }
            block_id = strtol(argv[2], NULL, 0);
            
        }
        break;
        
        case NORFLASH_CMD_WRITE:
        {
            if (argc < 5)
            {
                printk("%s\n", cmdtp->usage);
                return -1;
            }
            flash_addr = (unsigned int)strtol(argv[2], NULL, 0);
            if (flash_addr > NorFlash.flash_size)
            {
                printk("flash addr (0x%08x)is out range\n", flash_addr);
                return -1;
            }

            if (flash_addr & 3)
            {
                printk("flash addr (0x%08x)isn't aligned\n", flash_addr);
                return -1;
            }
            
            mem_addr = (unsigned int)strtol(argv[3], NULL, 0);
            if (mem_addr > 0x34000000)
            {
                printk("mem addr (0x%08x)is out range\n", mem_addr);
                return -1;
            }

            if (mem_addr & 3)
            {
                printk("mem addr (0x%08x)isn't aligned\n", mem_addr);
                return -1;
            }
            
            mem_size = (unsigned int)strtol(argv[4], NULL, 0);
            if (mem_size > 0x04000000)
            {
                printk("mem size (0x%08x)is out range\n", mem_size);
                return -1;
            }

            if (mem_size & 3)
            {
                printk("mem size (0x%08x)isn't aligned\n", mem_size);
                return -1;
            }
            
            norflash_write_buf(&NorFlash, flash_addr, mem_addr, mem_size);
        }
        break;
        default:
        {
            
        }
        break;
    }
    return 0;
}

TBOOT_CMD(norflash, 5, do_norflash, "usage:norflash <n> [flash addr/sector-id/block-id] [mem addr] [size]", 0);

