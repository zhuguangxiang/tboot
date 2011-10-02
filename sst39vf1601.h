/*
 * NOR-FLASH - SST39VF1601 DEVICE
 *
 */
 #ifndef _SST39VF1601_H_
 #define _SST39VF1601_H_

typedef struct nor_flash
{
    unsigned int base_address;
    unsigned short man_id;
    unsigned short dev_id;
    unsigned int flash_size;
    unsigned int sector_size;
    unsigned int block_size;
    
}NOR_FLASH;

#define NOR_FLASH_ADDRESS       0x0
#define NORFLASH_MAN_ID         0x00BF     
#define NORFLASH_DEV_ID         0x234B
#define NORFLASH_FLASH_SIZE     0x00200000      /* 2MB = 1MWord */
#define NORFLASH_SECTOR_SIZE    0x1000          /* 4KB = 2KWord */
#define NORFLASH_BLOCK_SIZE     0x10000         /* 64KB = 32KWord */

typedef enum
{
    SST39VF16001_PORT_1 = 0x5555,
    SST39VF16001_PORT_2 = 0x2aaa,
    
}NOR_FLASH_PORT;


#endif/*_SST39VF1601_H_*/
 
