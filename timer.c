
#include "s3c2440.h"
#include "io.h" 

static const int timer_load_val = 0xffff;

#define TIMER_CLOCK 1562500

/* using PWM Timer 4 because it has no output */
void sys_timer_init(void)
{
	inl(rTCFG0) |= (0xf << 8);                 /* prescaler = 16 */
    /* inl(rTCFG1) |= (1 << 16); */            /* divider(default) = 1/2 */
	
	/* timer clock = 50Mhz / (16 * 2) = 1.5625MHz */
	/* PCLK = 50MHz */
	
	/*
	 * for 10 ms clock period @ PCLK with 4 bit divider = 1/2
	 * (default) and prescaler = 16. Should be 10390
	 * @33.25MHz and 15625 @ 50 MHz
	 */
    inl(rTCNTB4) = (timer_load_val & 0xffff);		/* 16 bits counting */
	
    /* autoload, manual update bit */
    inl(rTCON) |= 0x00600000; 

    /* start timer0, auto-reload and clear manual update bit */
    inl(rTCON) = 0x00500000; 

}

/* the range of msec is less than 1375s */
void sys_timer_mdelay(int msec)
{
	int tmo = 0;				
	int now = 0, last = 0;
	
	if (msec > 1000)
	{
		tmo = msec / 1000;
		tmo = tmo * TIMER_CLOCK;		/* tmo = (msec * TIMER_CLOCK)/1000; */
	}
	else
	{
		tmo = (msec * TIMER_CLOCK);
		tmo = tmo / 1000;
	}

	last = inl(rTCNTO4) & 0xffff;
	
	while (tmo > 0)
	{
		now = inl(rTCNTO4) & 0xffff;
		
		if (now > last)
		{
			/* counting down timer overflow */
			tmo -= (timer_load_val + last - now);
		}
		else
		{
			tmo -= (last - now); 
		}
		
		last = now;
	}
}

/* the range of msec is less than 1375ms */
void sys_timer_udelay(int usec)
{
	int tmo = 0;				
	int now = 0, last = 0;
	
	if (usec > 1000)
	{
		tmo = usec / 1000;
		tmo = tmo * TIMER_CLOCK;		/* tmo = (msec * TIMER_CLOCK)/1000000;  */
		tmo = tmo / 1000;
	}
	else
	{
		tmo = (usec * TIMER_CLOCK);
		tmo = tmo / 1000000;
	}
	
	last = inl(rTCNTO4) & 0xffff;
	
	while (tmo > 0)
	{
		now = inl(rTCNTO4) & 0xffff;
		
		if (now > last)
		{
			/* counting down timer overflow */
			tmo -= (timer_load_val + last - now);
		}
		else
		{
			tmo -= (last - now); 
		}
		
		last = now;
	}
}

