OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
    . = 0x00000000; /* instead of LDFLAGS -Ttext */
    .text :
    {
    	  boot.o(.text)
        *(.text)        
    }
    . = ALIGN(4);
    .rodata :
    {
        *(.rodata)
    }
    . = ALIGN(4);
    .data :
    {
        *(.data)
    }
    . = ALIGN(4);
    __cmd_start = .;
  	.cmd_tbl : 
  	{ 
  		*(.cmd_tbl_item)
  	}
 	  __cmd_end = .;
 	
    end = .;
    . = ALIGN(4);
    __bss_start = .;
    .bss :
    {
        *(.bss)
    }
    __bss_end = .;
}