/*
 * boot.S
 *
 *  Created on: 2011-1-25
 *      Author: ForrestChu
 */
#include "s3c2440.h"

/* ARM920T using 5 pipe line, but pc is the same with ARM720T. */
#define PIPELINE    8

    .section ".text", #alloc, #execinstr
    .align 2
    .type _start, #function
    .global _start
_start:
	
	adr	r0, _start					/* r0 <- current position of code, sub	r0, pc, #8  */
	ldr	r1, _TEXT_BASE				/* test if we run from flash or RAM */
	cmp     r0, r1                  /* don't reloc if equal */
	beq     clear_bss
	
    /* Disable watch dog timer */
    mov r1, #0x53000000
    mov r0, #0x0
    str r0, [r1]

    /* disable all interrupts */
    mov r1, #INT_BASE
    mvn r0, #0x0
    str r0, [r1, #oINTMSK]

    mov r0, #0x700
    add r0, r0, #0xff            /* 0x7ff is not valid constant in ARM */
    str r0, [r1, #oINTSUBMSK] 
    
    /* Initialise system clocks */
    mov r1, #CLOCK_BASE
    ldr r0, [pc, #LockTimeAddr-.-8]
    str r0, [r1, #oLOCKTIME]

    ldr r0, [pc, #ClockDVinAddr-.-8]
    str r0, [r1, #oCLKDIVN]

    mrc p15, 0, r0, c1, c0, 0               /* read cp15 register 1  */
    orr r0, r0, #0xc0000000                 /* Asynchronous          */
    mcr p15, 0, r0, c1, c0, 0               /* write cp15 register 1 */

    ldr r0, [pc, #MpllAddr-.-8]       /* set MPLLCON */
    str r0, [r1, #oMPLLCON]

    /* Initialise SDRAM Controller */
    add r1, pc, #MEMCfgAddr-.-8
    mov r2, #MEMORY_BASE
    add r3, pc, #MEMCfgEnd-.-8
    
1:  ldr r0, [r1], #4
    str r0, [r2], #4
    cmp r3, r1
    bhi 1b

	/* move to ram */
	adr	r0, _start					/* r0 <- current position of code, sub	r0, pc, #112(0x70)  */
	ldr	r1, _TEXT_BASE				/* test if we run from flash or RAM */
	cmp     r0, r1                  /* don't reloc if equal */
	beq     turn_on_led

	ldr	r2, _TEXT_BASE
	ldr	r3, _bss_start
	sub	r2, r3, r2					/* r2 <- size of code + data        */
	add r2, r2, r0					/* r2 <- source end address         */
	
copy_loop:
	ldmia	r0!, {r3-r10}			/* copy from source address [r0]    */
	stmia	r1!, {r3-r10}			/* copy to   target address [r1]    */
	cmp	r0, r2						/* until source end addreee [r2]    */
	ble	copy_loop
	
clear_bss:	
	/* clear bss */
	ldr r2, _bss_start
	ldr r3, _bss_end
	mov r0, #0
	cmp r3, r2
	bls jump_to_ram
	
clear_loop:
	str r0, [r2], #4
	cmp r3, r2
	bhi clear_loop
	
jump_to_ram:
	/* set svc sp stack */
	mov sp, #0x30000000		/*low 32KB*/
	add sp, sp, #0x8000
	
	ldr pc, _start_boot
_start_boot:
	.word turn_on_led
	
turn_on_led:	
    /* Turn on Leds */
    mov r1, #GPIO_BASE
    add r1, r1, #oGPIO_B

    /* Set bits 5-8 low */
    ldr r0, [r1, #GPIO_DATA]
    bic r0, r0, #0x1e0
    str r0, [r1, #GPIO_DATA]

    /* Set bits 5-8 output */
    ldr r0, [r1, #GPIO_CTRL]
    mov r0, #0x15400
    str r0, [r1, #GPIO_CTRL]

    /* pull-up use default, enabled */

    /* delay for a while */
	
	
	bl sys_timer_init
	
	mov r0, #8000		/*delay 8s*/
	
	bl sys_timer_mdelay
	
turn_off_led:
	/* Turn on Leds */
    mov r1, #GPIO_BASE
    add r1, r1, #oGPIO_B
	
    /*  Turn off Leds */
    ldr r0, [r1, #GPIO_DATA]
    orr r0, r0, #0x1e0
    str r0, [r1, #GPIO_DATA]

    bl serial_init
	
    add r0, pc, #boot_string-.-8
	
    bl serial_puts

	b shell_loop
	
    /* SDRAM Configuration Values */
MEMCfgAddr:
    .word   vBWSCON
    .word   vBANKCON0
    .word   vBANKCON1
    .word   vBANKCON2
    .word   vBANKCON3
    .word   vBANKCON4
    .word   vBANKCON5
    .word   vBANKCON6
    .word   vBANKCON7
    .word   vREFRESH_100
    .word   vBANKSIZE
    .word   vMRSR6
    .word   vMRSR7
MEMCfgEnd:
    .word   0x0
    /* Processor Clock Values */
LockTimeAddr:
    .word   vLOCKTIME
ClockDVinAddr:
    .word   vCLKDVIN_148
MpllAddr:
    .word   vMPLL_400MHz
boot_string:
    .string "TinyBoot started......\n"
	
/*
 * These are defined in the board-specific linker script.
 */
_TEXT_BASE:
	.word	TEXT_BASE
_bss_start:
	.word __bss_start
_bss_end:
	.word __bss_end
    
