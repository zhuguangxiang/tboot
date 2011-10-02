/*
 * s3c2440.h
 *
 *  Created on: 2011-1-25
 *      Author: ForrestChu
 */

#ifndef _S3C2440_H_
#define _S3C2440_H_
#include "io.h"

/*****************************************************************************/
/*                             ARM920T  CPU                                  */
/*        |----|----|----|----|-----|---|---|---|---|---|---|---|---|        */
/*        | 31 | 30 | 29 | 28 | ... | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |        */
/*        |----|----|----|----|-----|---|---|---|---|---|---|---|---|        */
/*        |  N |  Z |  C |  V | ... | I | F | T | M4| M3| M2| M1| M0|        */
/*        |----|----|----|----|-----|---|---|---|---|---|---|---|---|        */
/*                                                                           */
/*****************************************************************************/
/* cpsr/spsr */
#define USR_MODE    0x10
#define FIQ_MODE    0x11
#define IRQ_MOD     0x12
#define SVC_MODE    0x13
#define ABT_MODE    0x17
#define UND_MODE    0x1b
#define SYS_MODE    0x1f
#define MODE_MASK   0x1f

#define BIT_F       0x40
#define BIT_T       0x80

/* control bits */
#define BIT_V        (1 << 28)
#define BIT_C        (1 << 29)
#define BIT_Z        (1 << 30)
#define BIT_N        (1 << 31)


/*****************************************************************************/
/*                      CLOCK & POWER MANAGEMENT                             */
/*                                                                           */
/*****************************************************************************/
#define CLOCK_BASE          0x4c000000

/* offset */
#define oLOCKTIME   0x00    /* PLL lock time count register */
#define oMPLLCON    0x04    /* MPLL configuration register */
#define oUPLLCON    0x08    /* UPLL configuration register */
#define oCLKCON     0x0C    /* Clock generator control register */
#define oCLKSLOW    0x10    /* Slow clock control register */
#define oCLKDIVN    0x14    /* Clock divider control register */
#define oCAMDIVN    0x18    /* Camera clock divider control register */

/* registers */
#define rLOCKTIME   inl(CLOCK_BASE+oLOCKTIME)
#define rMPLLCON    inl(CLOCK_BASE+oMPLLCON)
#define rUPLLCON    inl(CLOCK_BASE+oUPLLCON)
#define rCLKCON     inl(CLOCK_BASE+oCLKCON)
#define rCLKSLOW    inl(CLOCK_BASE+oCLKSLOW)
#define rCLKDIVN    inl(CLOCK_BASE+oCLKDIVN)
#define rCAMDIVN    inl(CLOCK_BASE+oCAMDIVN)

/* configued value */
/**************************************************************/
/*                      PLL LOCK TIME REG                     */
/*         31                16 15                 0          */
/*        |--------------------|--------------------|         */
/*        |       U_LTIME      |      M_LTIME       |         */
/*        |--------------------|--------------------|         */
/**************************************************************/
#define vULTIME  0xFF
#define vMLTIME  0xFFFF
#define vLOCKTIME   ( (vULTIME<<16) | (vMLTIME) )

/**************************************************************/
/*              PLL CONTROL REG(MPLL & UPLL)                  */
/*         31   20 19   12 11 10 9    4 3 2 1 0               */
/*        |-------|-------|-----|------|---|----|             */
/*        |       | MDIV  |     | PDIV |   |SDIV|             */
/*        |-------|-------|-----|------|---|----|             */
/**************************************************************/
/*
  at mini2440 board, external oscillator = Fin = 12MHz
  FCLK used by ARM920T
  HCLK used by AHB
  PCLK used by APB
  FCLK = Mpll = (2 * m * Fin)/(p * 2^s)
  Upll = (2 * m * Fin)/(p * 2^s)
  m = (MDIV + 8), p = (PDIV + 2), s = SDIV

  Assume using FCLK = 400.00MHz
  FCLK = Mpll = (2 * m * Fin)/(p * 2^s)
  then (2 * (92+8) * 12MHz)/((1+2)*2^1) = 200 * 12MHz / 6 = 400 MHz
 */
#define vMDIV   92
#define vPDIV   1
#define vSDIV   1
#define vMPLL_400MHz  ( (vMDIV<<12) | (vPDIV<<4) | (vSDIV) )

/**************************************************************/
/*        CLOCK DIVIDER CONTROL (CLKDIVN) REGISTER            */
/*         31           4     3     2   1   0                 */
/*        |--------------|---------|-----|-----|              */
/*        |              |DIVN_UPLL|HDIVN|PDIVN|              */
/*        |--------------|---------|-----|-----|              */
/**************************************************************/
/*
  DIVN_UPLL used by USB
  HDIVN used by HCLK(AHB)
  PDIVH used by PCLK(APB)
  AHB:
     ARM920T, the memory controller, the interrupt controller,
     the LCD controller, the DMA and USB host.
  APB:
    the peripherals such as
    WDT, IIS, I2C, PWM timer, MMC interface, ADC, UART, GPIO, RTC and SPI.
  HDIVN:
    0b00 HCLK = FCLK/1
    0b01 HCLK = FCLK/2
    0b10 HCLK = FCLK/4,when CAMDIVN[9] = 0.
         HCLK = FCLK/8,when CAMDIVN[9] = 1.
    0b10 HCLK = FCLK/3,when CAMDIVN[8] = 0.
         HCLK = FCLK/6,when CAMDIVN[8] = 1.
  PDIVN:
    0b0 PCLK = HCLK
    0b1 PCLK = HCLK/2
  Assume using FCLK = 400.00MHz HCLK = 100.00MHz PCLK = 50.00MHz
  then HCLK = FCLK/4, PCLK = HCLK/2=FCLK/8, rCAMDIVN using default value.
  FCLK:HCLK:PCLK = 1:4:8
 */
#define vDIVN_UPLL  0x0
#define vHDIVN      0x2     /* HCLK = FCLK/4 */
#define vPDIVN      0x1     /* PCLK = HCLK/2 = FCLK/8 */
#define vCLKDVIN_148    ( (vDIVN_UPLL<<3) | (vHDIVN<<1) | (vPDIVN) )


/**************************************************************/
/*           CLOCK CONTROL REGISTER (CLKCON)                  */
/*every bit controls one device                               */
/*and all bits are used default values is ok                  */
/**************************************************************/



/*****************************************************************************/
/*                      MEMORY CONTROLLER                                    */
/*                                                                           */
/*****************************************************************************/
#define MEMORY_BASE         0x48000000

/* offset */
#define oBWSCON     0x00    /* Bus width & wait status control register */
#define oBANKCON0   0x04    /* Bank 0 control register */
#define oBANKCON1   0x08    /* Bank 1 control register */
#define oBANKCON2   0x0c    /* Bank 2 control register */
#define oBANKCON3   0x10    /* Bank 3 control register */
#define oBANKCON4   0x14    /* Bank 4 control register */
#define oBANKCON5   0x18    /* Bank 5 control register */
#define oBANKCON6   0x1c    /* Bank 6 control register */
#define oBANKCON7   0x20    /* Bank 7 control register */
#define oREFRESH    0x24    /* SDRAM refresh control register */
#define oBANKSIZE   0x28    /* Flexible bank size register */
#define oMRSRB6     0x2c    /* Mode register set register bank 6 */
#define oMRSRB7     0x30    /* Mode register set register bank 7 */

/* configued value */
/*
 * s3c2440A has 8 banks with 1G address space between 0x00 and 0x3FFF_FFFF.
 * From nor flash startup, bank0 to bank5 connects SRAM or Nor Flash
 * (Direct Access Device).
 * bank6-bank7 can connect DAD or SDRAM
 *
 * mini2440 board(nor flash boot):
 * bank0 connects 2MB Nor Flash(0x00)
 * bank6 connects 64MB SDRAM(0x3000_0000)
 *
 */

/**************************************************************/
/*      BUS WIDTH & WAIT CONTROL REGISTER (BWSCON)            */
/**************************************************************/
/*  DW0: bank0 data width,readonly, controlled by OM[1:0]
        mini2440 OM[1:0] = 0b01 means
        boot from nor and bank0 width is 16bits
    DWi(i = 0 to 7): data width configuration
    WSi:WAIT signal enabled bit
    STi:determine sram for banki using UB/LB

 */
/* bank1 to bank5 not used, using 16bits */
#define vB0_BWSCON  0x0     /* bus width = 16bits, SST39VF1601 */
#define vB1_BWSCON  0x1
#define vB2_BWSCON  0x1
#define vB3_BWSCON  0x1
#define vB4_BWSCON  0x1     /* right? because DM9000 is in this space */
#define vB5_BWSCON  0x1
#define vB6_BWSCON  0x2     /* SDRAM(MT48LC16M16A2-7E),32bits */
#define vB7_BWSCON  0x2     /* not used */
#define vBWSCON \
    ( (vB1_BWSCON<<4) | (vB2_BWSCON<<8) | (vB3_BWSCON<<12) \
  | (vB4_BWSCON<<16) | (vB5_BWSCON<<20) | (vB6_BWSCON<<24) | (vB7_BWSCON<<28) )

/**************************************************************/
/*      BANK CONTROL REGISTER (BANKCONN: NGCS0-NGCS5)         */
/**************************************************************/
/* bank0-bank5 use default value for sram or nor flash */
#define vBANKCON0       0x00000700  /* SST39VF1601 */
#define vBANKCON1       0x00000700  /* not used */
#define vBANKCON2       0x00000700  /* not used */
#define vBANKCON3       0x00000700  /* not used */
#define vBANKCON4       0x00000700  /* DM9000 */
#define vBANKCON5       0x00000700  /* not used */

/**************************************************************/
/*      BANK CONTROL REGISTER (BANKCONn: nGCS6-nGCS7)         */
/**************************************************************/
/* bank6-bank7 SDRAM */
#define vMT     0x3
#define vTrcd   0x1     /* 3clock, maybe 2clock is OK */
#define vSCAN   0x1     /* column number is 9 for MT48LC16M16A2-7E */
#define vBANKCON6       (vSCAN | vTrcd<<2 | vMT<<15) /* SDRAM,64MB */
#define vBANKCON7       (vSCAN | vTrcd<<2 | vMT<<15) /* not used */


/**************************************************************/
/*              REFRESH CONTROL REGISTER                      */
/**************************************************************/
/* for SDRAM */
#define vREFEN  0x1     /* Refresh enable */
#define vTREFMD 0x0     /* CBR(CAS before RAS)=Auto refresh mode */
#define vTrp    0x1     /* 2clocks is OK */
#define vTsrc   0x1     /* 5clocks src+Trp = Trc = 60ns, Trp = 15ns, so Tsrc=45ns */
#define vTchr	0x2		/* 3clk */
#define vREFCNT 1268    /* for HCLK=100MHz */
#define vREFRESH_100    (vREFCNT | vTchr<<16 | vTsrc<<18 | vTrp<<20 | vTREFMD<<22 | vREFEN<<23)


/**************************************************************/
/*                  BANKSIZE REGISTER                         */
/**************************************************************/
/* for SDRAM */
#define vBANKSIZE   0xb1    /* SCLK power saving mode, BANKSIZE 64M/64M */

/**************************************************************/
/*          SDRAM MODE REGISTER SET REGISTER (MRSR)           */
/**************************************************************/
/* fro SDRAM */
#define vMRSR6      0x30    /* CL = 3 clocks */
#define vMRSR7      0x30

/*****************************************************************************/
/*                   INTERRUPT CONTROLLER                                    */
/*                                                                           */
/*****************************************************************************/
#define INT_BASE        0x4a000000

/* offset */
#define oSRCPND         0x00
#define oINTMOD         0x04        /* default 0x00 IRQ mode(FIQ mode) */
#define oINTMSK         0x08
#define oPRIORITY       0x0c        /* default 0x7F */
#define oINTPND         0x10
#define oINTOFFSET      0x14        /* read only */
#define oSUBSRCPND      0x18
#define oINTSUBMSK      0x1c

/* registers */
#define rSRCPND         inl(INT_BASE+oSRCPND)
#define rINTMOD         inl(INT_BASE+oINTMOD)       /* not used */
#define rINTMSK         inl(INT_BASE+oINTMSK)
#define rPRIORITY       inl(INT_BASE+oPRIORITY)     /* not used */
#define rINTPND         inl(INT_BASE+oINTPND)
#define rINTOFFSET      inl(INT_BASE+oINTOFFSET)    /* read only */
#define rSUBSRCPND      inl(INT_BASE+oSUBSRCPND)
#define rINTSUBMSK      inl(INT_BASE+oINTSUBMSK)

/*
request source----------------------------|
(without sub-register)                    |
                             |---------|  V            |--------|
request source-->SUBSRCPND-->|INTSUBMSK|----->SRCPND-->| INTMSK |-->INTPND-->IRQ
(with sub-register)          |---------|   ^           |--------|
                                           |
                             |---------|   |
external request-->EINTPND-->| EINTMSK |---|
                             |---------|

when has a request, the SRCPND corresponding bit set 1, then check INMSK,set INTPND.
sub request is the same.

internal interrupt:
without sub interrupt:INTMSK
with sub interrupt:INTSUBMSK, INTMSK

external interrupt:
EINT0-3: SRCPND & INTMSK
EINT4-7: EINTPND & EINTMSK  --> SRCPND & INTMSK
EINT8-23: EINTPND & EINTMSK --> SRCPND & INTMSK
handle interrupt:
1.get rINTOFFSET
2.case handle it
3.clear interrupt:
3.1 no sub(include EINT0-3):
    SRCPND--->INTPND
3.2 has sub(include EINT4-23):
    SUBSRCPND(EINTPND)--->SRCPND--->INTPND
3.3 method:
    write 1 to corresponding bit(SUBSRCPND,EINTPND,SRCPND,INTPND)
 */

/* PENDING BIT */
#define BIT_ADC         (1 << 31)   /* ADC EOC interrupt */
#define BIT_RTC         (1 << 30)   /* RTC alarm interrupt */
#define BIT_SPI1        (1 << 29)   /* UART1 transmit interrupt */
#define BIT_UART0       (1 << 28)   /* UART0 transmit interrupt */
#define BIT_IIC         (1 << 27)   /* IIC interrupt */
#define BIT_USBH        (1 << 26)   /* USB host interrupt */
#define BIT_USBD        (1 << 25)   /* USB device interrupt */
#define BIT_NFCON       (1 << 24)
#define BIT_UART1       (1 << 23)   /* UART1 receive interrupt */
#define BIT_SPI0        (1 << 22)   /* SPI interrupt */
#define BIT_SDI         (1 << 21)   /* SD interrupt */
#define BIT_DMA3        (1 << 20)   /* DMA channel 3 interrupt */
#define BIT_DMA2        (1 << 19)   /* DMA channel 2 interrupt */
#define BIT_DMA1        (1 << 18)   /* DMA channel 1 interrupt */
#define BIT_DMA0        (1 << 17)   /* DMA channel 0 interrupt */
#define BIT_LCD         (1 << 16)   /* reserved for future use */
#define BIT_UART2       (1 << 15)   /* UART 2 interrupt  */
#define BIT_TIMER4      (1 << 14)   /* Timer 4 interrupt */
#define BIT_TIMER3      (1 << 13)   /* Timer 3 interrupt */
#define BIT_TIMER2      (1 << 12)   /* Timer 2 interrupt */
#define BIT_TIMER1      (1 << 11)   /* Timer 1 interrupt */
#define BIT_TIMER0      (1 << 10)   /* Timer 0 interrupt */
#define BIT_WDT_AC97    (1 << 9)    /* Watch-Dog timer interrupt */
#define BIT_TICK        (1 << 8)    /* RTC time tick interrupt  */
#define BIT_BAT_FLT     (1 << 7)
#define BIT_CAM         (1 << 6)
#define BIT_EINT8_23    (1 << 5)    /* External interrupt 8 ~ 23 */
#define BIT_EINT4_7     (1 << 4)    /* External interrupt 4 ~ 7 */
#define BIT_EINT3       (1 << 3)    /* External interrupt 3 */
#define BIT_EINT2       (1 << 2)    /* External interrupt 2 */
#define BIT_EINT1       (1 << 1)    /* External interrupt 1 */
#define BIT_EINT0       (1 << 0)    /* External interrupt 0 */

#define BIT_SUB_AC97        (1 << 14)
#define BIT_SUB_WDT         (1 << 13)   /* watch dog */
#define BIT_SUB_CAM_P       (1 << 12)
#define BIT_SUB_CAM_C       (1 << 11)
#define BIT_SUB_ADC_S       (1 << 10)
#define BIT_SUB_TC          (1 << 9)
#define BIT_SUB_ERR2        (1 << 8)    /* uart2 */
#define BIT_SUB_TXD2        (1 << 7)
#define BIT_SUB_RXD2        (1 << 6)
#define BIT_SUB_ERR1        (1 << 5)    /* uart1 */
#define BIT_SUB_TXD1        (1 << 4)
#define BIT_SUB_RXD1        (1 << 3)
#define BIT_SUB_ERR0        (1 << 2)    /* uart0 */
#define BIT_SUB_TXD0        (1 << 1)
#define BIT_SUB_RXD0        (1 << 0)


/*****************************************************************************/
/*                        UART CONTROLLER                                    */
/*                                                                           */
/*****************************************************************************/
/* UART */
#define UART_CTL_BASE		0x50000000

#define UART0_CTL_BASE		UART_CTL_BASE
#define UART1_CTL_BASE		UART_CTL_BASE + 0x4000
#define UART2_CTL_BASE		UART_CTL_BASE + 0x8000
#define bUART(x, Nb)		inl(UART_CTL_BASE + (x)*0x4000 + (Nb))
#define bUARTb(x, Nb)		inb(UART_CTL_BASE + (x)*0x4000 + (Nb))
/* Offset */
#define oULCON			0x00	/* R/W, UART line control register */
#define oUCON			0x04	/* R/W, UART control register */
#define oUFCON			0x08	/* R/W, UART FIFO control register */
#define oUMCON			0x0C	/* R/W, UART modem control register */
#define oUTRSTAT		0x10	/* R  , UART Tx/Rx status register */
#define oUERSTAT		0x14	/* R  , UART Rx error status register */
#define oUFSTAT			0x18	/* R  , UART FIFO status register */
#define oUMSTAT			0x1C	/* R  , UART Modem status register */
#define oUTXHL			0x20	/* W  , UART transmit(little-end) buffer */
#define oUTXHB			0x23	/* W  , UART transmit(big-end) buffer */
#define oURXHL			0x24	/* R  , UART receive(little-end) buffer */
#define oURXHB			0x27	/* R  , UART receive(big-end) buffer */
#define oUBRDIV			0x28	/* R/W, Baud rate divisor register */

/* Registers */
#define rULCON0			bUART(0, oULCON)
#define rUCON0			bUART(0, oUCON)
#define rUFCON0			bUART(0, oUFCON)
#define rUMCON0			bUART(0, oUMCON)
#define rUTRSTAT0		bUART(0, oUTRSTAT)
#define rUERSTAT0		bUART(0, oUERSTAT)
#define rUFSTAT0		bUART(0, oUFSTAT)
#define rUMSTAT0		bUART(0, oUMSTAT)
#define rUTXH0			bUARTb(0, oUTXHL)
#define rURXH0			bUARTb(0, oURXHL)
#define rUTXHB0			bUARTb(0, oUTXHB)

#define rUBRDIV0		bUART(0, oUBRDIV)
#define rULCON1			bUART(1, oULCON)
#define rUCON1			bUART(1, oUCON)
#define rUFCON1			bUART(1, oUFCON)
#define rUMCON1			bUART(1, oUMCON)
#define rUTRSTAT1		bUART(1, oUTRSTAT)
#define rUERSTAT1		bUART(1, oUERSTAT)
#define rUFSTAT1		bUART(1, oUFSTAT)
#define rUMSTAT1		bUART(1, oUMSTAT)
#define rUTXH1			bUARTb(1, oUTXHL)
#define rURXH1			bUARTb(1, oURXHL)
#define rUBRDIV1		bUART(1, oUBRDIV)
#define rULCON2			bUART(2, oULCON)
#define rUCON2			bUART(2, oUCON)
#define rUFCON2			bUART(2, oUFCON)
#define rUMCON2			bUART(2, oUMCON)
#define rUTRSTAT2		bUART(2, oUTRSTAT)
#define rUERSTAT2		bUART(2, oUERSTAT)
#define rUFSTAT2		bUART(2, oUFSTAT)
#define rUMSTAT2		bUART(2, oUMSTAT)
#define rUTXH2			bUARTb(2, oUTXHL)
#define rURXH2			bUARTb(2, oURXHL)
#define rUBRDIV2		bUART(2, oUBRDIV)
/* ... */
#define UTRSTAT_TX_EMPTY	(1 << 2)
#define UTRSTAT_RX_READY	(1 << 0)
#define UART_ERR_MASK		0xF


/*****************************************************************************/
/*                             TIMER                                         */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                            GPIO                                           */
/*                                                                           */
/*****************************************************************************/
#define GPIO_BASE       0x56000000
/* offset */
#define oGPIO_A         0x00
#define oGPIO_B         0x10
#define oGPIO_C         0x20
#define oGPIO_D         0x30
#define oGPIO_E         0x40
#define oGPIO_F         0x50
#define oGPIO_G         0x60
#define oGPIO_H         0x70
#define oGPIO_J         0xd0

#define GPIO_CTRL       0x0     /* Configures the pins of the port */
#define GPIO_DATA       0x4     /* Data register for port */
#define GPIO_UP         0x8     /* Pull-up disable register */
#define RESERVED        0xc     /* Reserved */

/* other registers offset */
#define oMISCCR         0x80    /* Miscellaneous control register */
#define oDCLKCON        0x84    /* DCLK0/1 control register */
#define oEXTINT0        0x88    /* External interrupt control registe */
#define oEXTINT1        0x8c    /* External interrupt control registe */
#define oEXTINT2        0x90    /* External interrupt control registe */
#define oEINTFLT0       0x94    /* External interrupt filter control register */
#define oEINTFLT1       0x98    /* External interrupt filter control register */
#define oEINTFLT2       0x9c    /* External interrupt filter control register */
#define oEINTFLT3       0xa0    /* External interrupt filter control register */
#define oEINTMASK       0xa4    /* External interrupt mask register */
#define oEINTPEND       0xa8    /* External interrupt pending register */

#define oGSTATUS0       0xac    /* External pin status */
#define oGSTATUS1       0xb0    /* CHIP ID =0x32440001 */
#define oGSTATUS2       0xb4    /* Reset type */
/* Saved data0(32-bit) before entering POWER_OFF mode */
#define oGSTATUS3       0xb8
/* Saved data0(32-bit) before entering POWER_OFF mode */
#define oGSTATUS4       0xbc

/* Registers */
#define rGPACON         inl(GPIO_BASE+oGPIO_A+GPIO_CTRL)
#define rGPADAT         inl(GPIO_BASE+oGPIO_A+GPIO_DATA)

#define rGPBCON         inl(GPIO_BASE+oGPIO_B+GPIO_CTRL)
#define rGPBDAT         inl(GPIO_BASE+oGPIO_B+GPIO_DATA)
#define rGPBUP          inl(GPIO_BASE+oGPIO_B+GPIO_UP)

#define rGPCCON         inl(GPIO_BASE+oGPIO_C+GPIO_CTRL)
#define rGPCDAT         inl(GPIO_BASE+oGPIO_C+GPIO_DATA)
#define rGPCUP          inl(GPIO_BASE+oGPIO_C+GPIO_UP)

#define rGPDCON         inl(GPIO_BASE+oGPIO_D+GPIO_CTRL)
#define rGPDDAT         inl(GPIO_BASE+oGPIO_D+GPIO_DATA)
#define rGPDUP          inl(GPIO_BASE+oGPIO_D+GPIO_UP)

#define rGPECON         inl(GPIO_BASE+oGPIO_E+GPIO_CTRL)
#define rGPEDAT         inl(GPIO_BASE+oGPIO_E+GPIO_DATA)
#define rGPEUP          inl(GPIO_BASE+oGPIO_E+GPIO_UP)

#define rGPFCON         inl(GPIO_BASE+oGPIO_F+GPIO_CTRL)
#define rGPFDAT         inl(GPIO_BASE+oGPIO_F+GPIO_DATA)
#define rGPFUP          inl(GPIO_BASE+oGPIO_F+GPIO_UP)

#define rGPGCON         inl(GPIO_BASE+oGPIO_G+GPIO_CTRL)
#define rGPGDAT         inl(GPIO_BASE+oGPIO_G+GPIO_DATA)
#define rGPGUP          inl(GPIO_BASE+oGPIO_G+GPIO_UP)

#define rGPHCON         inl(GPIO_BASE+oGPIO_H+GPIO_CTRL)
#define rGPHDAT         inl(GPIO_BASE+oGPIO_H+GPIO_DATA)
#define rGPHUP          inl(GPIO_BASE+oGPIO_H+GPIO_UP)

#define rMISCCR         inl(GPIO_BASE+oMISCCR)
#define rDCLKCON        inl(GPIO_BASE+oDCLKCON)
#define rEXTINT0        inl(GPIO_BASE+oEXTINT0)
#define rEXTINT1        inl(GPIO_BASE+oEXTINT1)
#define rEXTINT2        inl(GPIO_BASE+oEXTINT2)
#define rEINTFLT0       inl(GPIO_BASE+oEINTFLT0)
#define rEINTFLT1       inl(GPIO_BASE+oEINTFLT1)
#define rEINTFLT2       inl(GPIO_BASE+oEINTFLT2)
#define rEINTFLT3       inl(GPIO_BASE+oEINTFLT3)
#define rEINTMASK       inl(GPIO_BASE+oEINTMASK)
#define rEINTPEND       inl(GPIO_BASE+oEINTPEND)

/*****************************************************************************/
/*                             TIMER                                         */
/*                                                                           */
/*****************************************************************************/
/* PWM Timer */
#define TIMER_BASE          0x51000000

#define bPWM_TIMER(Nb)      (TIMER_BASE + (Nb))
#define bPWM_BUFn(Nb,x)     bPWM_TIMER(0x0c + (Nb)*0x0c + (x))

/* Registers */
#define rTCFG0              bPWM_TIMER(0x00)
#define rTCFG1              bPWM_TIMER(0x04)
#define rTCON               bPWM_TIMER(0x08)

#define rTCNTB0             bPWM_BUFn(0,0x0)
#define rTCMPB0             bPWM_BUFn(0,0x4)
#define rTCNTO0             bPWM_BUFn(0,0x8)

#define rTCNTB1             bPWM_BUFn(1,0x0)
#define rTCMPB1             bPWM_BUFn(1,0x4)
#define rTCNTO1             bPWM_BUFn(1,0x8)

#define rTCNTB2             bPWM_BUFn(2,0x0)
#define rTCMPB2             bPWM_BUFn(2,0x4)
#define rTCNTO2             bPWM_BUFn(2,0x8)

#define rTCNTB3             bPWM_BUFn(3,0x0)
#define rTCMPB3             bPWM_BUFn(3,0x4)
#define rTCNTO3             bPWM_BUFn(3,0x8)

#define rTCNTB4             bPWM_BUFn(4,0x0)
#define rTCNTO4             bPWM_BUFn(4,0x4)
#endif /* S3C2440_H_ */
