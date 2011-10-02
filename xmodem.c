#include "xmodem.h"
#include "string.h"
#include "stddef.h"
#include "crc.h"

void xmdm_client_flush(XMDM_CLIENT *xmdm)
{
	int state = 0;
	char ch = 0;
	
	while (1)
	{
		state = xmdm->recv_char_tmo_func(&ch, xyzModem_CHAR_TIMEOUT);
		if (state == 0)
		{
			return;
		}
	}
}

void xmdm_client_init(XMDM_CLIENT *xmdm, bool using_crc, 
                       XMDM_SEND_CHAR_FUNC uart_send_char,
                       XMDM_RECV_CHAR_TMO_FUNC uart_recv_char_tmo)
{
   memset(xmdm, 0, sizeof *xmdm);
   xmdm->using_crc = using_crc;
   xmdm->send_char_func = uart_send_char;
   xmdm->recv_char_tmo_func = uart_recv_char_tmo;
}

int xmdm_client_open(XMDM_CLIENT *xmdm)
{
    char start_ch = (xmdm->using_crc == true) ? 'C' : NAK;
    
	xmdm->send_char_func(start_ch);
    xmdm->next_blk = 1;
    
	xmdm->pkt_len = 0;
    memset(xmdm->pkt, 0, XMDM_CLIENT_BUF_SIZE);
    
	return 0;
}

int xmdm_client_recv_pkt(XMDM_CLIENT *xmdm)
{
	int state = 0, can_total = 0;
	char ch = 0;
	bool hdr_found = false;
	int first_char = 0;
	int i = 0, len = 0;
	unsigned short cksum;

    xmdm->pkt_len = 0;
    memset(xmdm->pkt, 0, XMDM_CLIENT_BUF_SIZE);
    
	while (hdr_found == false)
	{
		state = xmdm->recv_char_tmo_func(&ch, xyzModem_CHAR_TIMEOUT);
		if (state == 1)
		{
			first_char++;
			switch (ch)
			{
				case SOH:
				{
					len = 128;
                    hdr_found = true;
				}
                break;
				case STX:
				{
					len = 1024;
                    hdr_found = true;
				}
				break;
				
				case CAN:
				{
                    ++can_total;
					if (can_total == xyzModem_CAN_COUNT)
					{
						return xyzModem_cancel;
					}
					else
					{
						/* Wait for multiple CAN to avoid early quits */
						break;
					}
				}
				break;

				case EOT:
				{  
					/* EOT only supported if no noise */
					if (first_char == 1)
					{
						return xyzModem_eof;
					}
				}
				break;
				default:
				{
					/* Ignore, waiting for start of header */
				}
				break;
			}
		}
		else if (state == 0)	/* timeout */
		{
			/* Data stream timed out */
			xmdm_client_flush(xmdm);		/* Toss any current input */
			return xyzModem_timeout;
		}
		else
		{
		}
	}
	
	/* Header found, now read the data */
	state = xmdm->recv_char_tmo_func(&ch, xyzModem_CHAR_TIMEOUT);
	if (state == 1)
	{
		xmdm->blk = ch;
	}
	else if (state == 0)
	{
		return xyzModem_timeout;
	}
	else
	{
	}
	
	state = xmdm->recv_char_tmo_func(&ch, xyzModem_CHAR_TIMEOUT);
	if (state == 1)
	{
		xmdm->cblk = ch;
	}
	else if (state == 0)
	{
		return xyzModem_timeout;
	}
	else
	{
	}
	
	for (i = 0; i < len; i++)
	{
		state = xmdm->recv_char_tmo_func(&ch, xyzModem_CHAR_TIMEOUT);
		if (state == 1)
		{
			xmdm->pkt[i] = ch;
            xmdm->pkt_len++;
		}
		else if (state == 0)
		{
			return xyzModem_timeout;
		}
		else
		{
		} 
	}
	
	state = xmdm->recv_char_tmo_func(&ch, xyzModem_CHAR_TIMEOUT);
	if (state == 1)
	{
		xmdm->crc1 = ch;
	}
	else if (state == 0)
	{
		return xyzModem_timeout;
	}
	else
	{
	}
	
	if (xmdm->using_crc == true)
	{
		state = xmdm->recv_char_tmo_func(&ch, xyzModem_CHAR_TIMEOUT);
		if (state == 1)
		{
			xmdm->crc2 = ch;
		}
		else if (state == 0)
		{
			return xyzModem_timeout;
		}
		else
		{
		}
	}
	
	/* Validate the message, only validate this packet itself */
	if ((xmdm->blk ^ xmdm->cblk) != (unsigned char) 0xFF)
    {
        xmdm_client_flush(xmdm);
        return xyzModem_frame;
    }
    
	/* Verify checksum/CRC */
    if (xmdm->using_crc == true)
    {
        cksum = cyg_crc16(xmdm->pkt, xmdm->pkt_len);
        if (cksum != ((xmdm->crc1 << 8) | xmdm->crc2))
        {
            return xyzModem_cksum;
        }
    }
    else
    {
        cksum = 0;
        
        for (i = 0; i < xmdm->pkt_len; i++)
        {
            cksum += xmdm->pkt[i];
        }
        
        if (xmdm->crc1 != (cksum & 0xFF))
        {
            return xyzModem_cksum;
        }
    }
    
    /* If we get here, the message passes [structural] muster */
    return 0;
}

int xmdm_client_read(XMDM_CLIENT *xmdm, char *buf, int *size, bool *eof)
{
    int state = 0, retries = xyzModem_MAX_RETRIES;

	while (1)
	{
		state = xmdm_client_recv_pkt(xmdm);
		if (state == 0)
		{
			if (xmdm->blk == xmdm->next_blk)
			{
				xmdm->next_blk = (xmdm->next_blk + 1) & 0xFF;
                
                /* Data blocks can be padded with ^Z (EOF) characters */
                /* This code tries to detect and remove them */
                if ((xmdm->pkt[xmdm->pkt_len - 1] == EOF) &&
                     (xmdm->pkt[xmdm->pkt_len - 2] == EOF) &&
                     (xmdm->pkt[xmdm->pkt_len - 3] == EOF))
                {
                    while ((xmdm->pkt_len > 0) && 
                            (xmdm->pkt[xmdm->pkt_len - 1] == EOF))
                    {
                        xmdm->pkt_len--;
                    }
                }

                memcpy (buf, xmdm->pkt, xmdm->pkt_len);
                *size = xmdm->pkt_len;
                xmdm->send_char_func(ACK);
                *eof = false;
                return 0;
                
			}
			else if (xmdm->blk == ((xmdm->next_blk - 1) & 0xFF))
			{
                /* duplicate packet */
				/* Just re-ACK this so sender will get on with it */
				xmdm->send_char_func(ACK);
				continue;	/* Need new header */
			}
			else
		    {
                /* packet seqence error */
                *eof = false;
				return xyzModem_sequence;
		    }
		}
		else if (state == xyzModem_cancel)
		{
            *eof = false;
            return xyzModem_cancel;
		}
		else if (state == xyzModem_eof)
		{
            xmdm->send_char_func(ACK);
            *eof = true;
            return 0;
		}
        else
        {
        }

        retries--;
        
        if (retries > 0)
        {
            char start_ch = (xmdm->using_crc == true) ? 'C' : NAK;
            xmdm->send_char_func(start_ch);
        }
        else
        {
            return xyzModem_retries;
        }
	}
    
}

