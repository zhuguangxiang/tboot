#include "command.h"
#include "stddef.h"
#include "uart.h"
#include "string.h"

extern void printk(const char *fmt, ...);

static const char vt100_bs_sequence[] = {0x1b, 0x5b, 0x44, 0x1b, 0x5b, 0x4b, 0};    /*esc [ D(cursor move one back) esc [ K(erase current to end)*/
static const char vt100_arrow_right_sequence[] = {0x1b, 0x5b, 0x43, 0}; 
static const char vt100_arrow_left_sequence[] = {0x1b, 0x5b, 0x44, 0}; 

#define RM_MODE_BYTE	1
#define RM_MODE_WORD	2

#define RM_DEFAULT_SIZE		32

void printMemory(char *address, int size, char mode)
{
	int *addr_32 = (int *)address;
	char *addr_8 = (char *)address;
	int size_32 = size / 4;
	int size_8 = size;
	int idx = 0;
	int count = 0;
	
	if (mode == RM_MODE_BYTE)
	{
		for (idx = 0; idx < size_8; idx++)
		{
			if (count == 0)
			{
				printk("0x%08x: ", addr_8 + idx);
			}
			else if (count == 16)
			{
				count = 0;
				printk("\n0x%08x: ", addr_8 + idx);
			}
			else
			{
			}
			
			count++;
			
			printk("%02x ", addr_8[idx]);

		}
	}
	else
	{
		for (idx = 0; idx < size_32; idx++)
		{
			if (count == 0)
			{
				printk("0x%08x: ", addr_32 + idx);
			}
			else if (count == 4)
			{
				count = 0;
				printk("\n0x%08x: ", addr_32 + idx);
			}
			else
			{
			}
			
			count++;
			
			printk("%08x ", addr_32[idx]);

		}
		
	}
	
	printk("\n");
}

extern long strtol(const char *cp, char **endp, unsigned int base);

int doReadMemory(struct cmd_table *cmdtp, int argc, char *argv[])
{
	int size = RM_DEFAULT_SIZE;
	char mode = RM_MODE_WORD;
	char  *address = NULL;
	int addr_int = 0;
	
	cmdtp = cmdtp;
	
	if (argc < 2)
	{
		printk("usage:md addr [size] [b/w]\n");
		return 0;
	}
	
	addr_int = strtol(argv[1], NULL, 0);
	
	address = (char *)(addr_int & ~15);
	
	if (argc > 2)
	{
		size = (int)strtol(argv[2], NULL, 0);
		if (size == 0)
		{
			size = RM_DEFAULT_SIZE;
		}
		size = (size + 31) & ~31;
	}
	
	if (argc > 3)
	{
		if (*(argv[3]) == 'b')
		{
			mode = RM_MODE_BYTE;
		}
		else
		{
			mode = RM_MODE_WORD;
		}
	}
	printMemory(address, size, mode);
	return 0;
}
 
 /*TBoot>md 0x100 100 [b/w]*/
TBOOT_CMD(md, 4, doReadMemory, "Display memory content", 0);

int do_version(struct cmd_table *cmdtp, int argc, char *argv[])
{
	cmdtp = cmdtp;
	argc = argc;
	argv = argv;
	
	printk(CONFIG_SYS_VERSION);
	return 0;
}
 
TBOOT_CMD(version, 1, do_version, "Show TinyBoot Version", 0);

typedef void (*REBOOT_ENTRY)(void);
void sys_timer_mdelay(int msec);
int do_reboot(struct cmd_table *cmdtp, int argc, char *argv[])
{
    REBOOT_ENTRY reboot = NULL;
    int count = 5;
	cmdtp = cmdtp;
	argc = argc;
	argv = argv;
	
	printk("system is rebooting...");
    while (count > 0)
    {
        printk("%d", count);
        sys_timer_mdelay(1000);
        serial_puts(vt100_bs_sequence);
        count--;
    }
    
    serial_putc('0');
    serial_putc('\n');
    
    sys_timer_mdelay(20);
    
    reboot();
	return 0;
}
 
TBOOT_CMD(reboot, 1, do_reboot, "reboot system", 0);

/**************************************************************************************************/

int do_auto_completion_test(struct cmd_table *cmdtp, int argc, char *argv[])
{
	cmdtp = cmdtp;
	argc = argc;
	argv = argv;

	printk(CONFIG_SYS_VERSION"do_auto_completion_test\n");
	return 0;
}
 
TBOOT_CMD(auto_completion_test, 1, do_auto_completion_test, "Show TinyBoot Version", 0);

void cmdTblIterate(void (*callFunc)(void *));

void printCmdHelp(void *pArg)
{
    struct cmd_table *cmdtp = (struct cmd_table *)pArg;
    printk("%s -- %s\n", cmdtp->name, cmdtp->usage);
}

int do_help(struct cmd_table *cmdtp, int argc, char *argv[])
{
	cmdtp = cmdtp;
	argc = argc;
	argv = argv;

	cmdTblIterate(printCmdHelp);
    
	return 0;
}
 
TBOOT_CMD(help, 1, do_help, "show commands", 0);

#define CMD_GET_CHAR_TIMEOUT	1	/* 2 second */

#define KEY_UP      1
#define KEY_DOWN    2
#define KEY_RIGHT   3
#define KEY_LEFT    4

int parse_vt100_esc(void)
{
	char ch = 0;
	ch = serial_getc(); 		/* read a character from uart with timeout */
	
	if (ch == '[')
	{
		ch = serial_getc();
		
		if (ch == 'A')
		{
			return KEY_UP;
		}
		else
		{
			printk("2===%d===\n", ch);
			return -1;
		}
	}
	else
	{
		printk("1===%d===\n", ch);
		return -1;
	}
}

int parse_vt100_esc_timeout(void)
{
	char ch = 0;
	int state = 0;
	
	state = serial_getc_timeout(&ch, CMD_GET_CHAR_TIMEOUT); 		/* read a character from uart with timeout */
	if (state == 0)
	{
		serial_puts("timeout\n");
		return -1;
	}
	
	if (ch == '[')
	{
		state = serial_getc_timeout(&ch, CMD_GET_CHAR_TIMEOUT); 		/* read a character from uart with timeout */
		
		if (state == 0)
		{
			serial_puts("timeout again\n");
			return -1;
		}

		if (ch == 'A')
		{
			return KEY_UP;
		}
		else if(ch == 'B')
		{
			return KEY_DOWN;
		}
        else if(ch == 'C')
        {
            return KEY_RIGHT;
        }
        else if(ch == 'D')
        {
            return KEY_LEFT;
        }
        else
        {
            return -1;
        }
	}
	else
	{
		return -1;
	}
}

void handle_vt100_backspace(void)
{
	serial_puts(vt100_bs_sequence);
}

extern struct cmd_table __cmd_start;
extern struct cmd_table __cmd_end;

struct cmd_table *find_cmd(const char *cmd, bool *isFullMatch);

void auto_completion(char *cmd_buf, int size, char **ptr, char *full_name)
{
	size = size;
	size_t idx = strlen(cmd_buf);
	while (full_name[idx] != 0)
	{
		cmd_buf[idx] = full_name[idx];
		idx++;
		(*ptr)++;
	}
}

#define HISTORY_CMD_COUNT   32
char *history_cmd[HISTORY_CMD_COUNT];
int history_count = 0;
int history_rover = -1;

void history_cmd_proc(int up_down)
{
    if (up_down == KEY_UP)
    {
        if ((history_rover > -1) && (history_rover < 32))
        {
            printk("%s\n", history_cmd[history_rover]);

            history_rover--;
            if (history_rover <= -1)
            {
                history_rover = history_count - 1;
            }
        }
    }
    else if(up_down == KEY_DOWN)
    {
        if ((history_rover > -1) && (history_rover < 32))
        {
            printk("%s\n", history_cmd[history_rover]);

            history_rover++;
            if (history_rover >= history_count+1)
            {
                history_rover = 0;
            }
        }
    }
    else
    {
        
    }
}

/*
 * Prompt for input and read a line.
 * If  CONFIG_BOOT_RETRY_TIME is defined and retry_time >= 0,
 * time out when time goes past endtime (timebase time in ticks).
 * Return:	number of read characters
 *		-1 if break
 *		-2 if timed out
 */
int readline(const char *const prompt, char *cmd_buf, int size)
{
	char ch = 0;
	char *ptr = cmd_buf, *ptr_end = cmd_buf + size, *ptr_start = NULL;
	
	struct cmd_table *full_cmd_tblp = NULL;
	
	if (prompt != NULL)
	{
		serial_puts(prompt);
	}
	
	while (1)
	{
		ch = serial_getc();	  /* blocked read a character from uart */
		switch (ch)
		{
			case '\r':
			case '\n':
			{
				serial_putc('\n');
				
				if (ptr >= ptr_end)
				{
					ptr--;
				}
				*ptr = 0;
				return (ptr - cmd_buf);
			}
			break;
			case 0x08:	/* backspace */
			{
				if (ptr > cmd_buf)		/* cmd_buf is empty, so not allowed erase a char */
				{
					*(--ptr) = 0;
					handle_vt100_backspace();
				}
			}
			break;
			case '\t':
			{
				*ptr = 0;
				full_cmd_tblp = find_cmd(cmd_buf, NULL);
				if (full_cmd_tblp != NULL)
				{
					ptr_start = ptr;
					auto_completion(cmd_buf, size, &ptr, full_cmd_tblp->name);
					while (ptr_start < ptr)
					{
						serial_putc(*ptr_start++);
					}
				}
			}
			break;
			
			case 0x1b: 	/*ESC sequence*/
			{
				switch (parse_vt100_esc_timeout())
				{
					case KEY_UP:
					{
						history_cmd_proc(KEY_UP);
					}
					break;
                    case KEY_DOWN:
					{
						history_cmd_proc(KEY_DOWN);
					}
					break;
                    case KEY_RIGHT:
					{
						serial_puts(vt100_arrow_right_sequence);
					}
					break;
                    case KEY_LEFT:
					{
						serial_puts(vt100_arrow_left_sequence);
					}
					break;
					default:
					{
						serial_putc('N');
						serial_putc('O');
					}
					break;
				}
			}
			break;
			case 3:		/* ctrl + c */
			{
				serial_putc('\n');
                if (prompt != NULL)
            	{
            		serial_puts(prompt);
            	}
			}
			break;
			default:
			{
				serial_putc(ch);
				
				if (ptr < ptr_end)
				{
					*ptr++ = ch;
				}
				else
				{
					ptr--;
					*ptr = 0;
					return (ptr - cmd_buf);
				}
			}
			break;
		}
	}
}

/* split the line string using token to the argv */
int parse_line(char *cmd_buf, char token, char *argv[])
{
	char *start = cmd_buf;
	char *end = cmd_buf;
	char ch = *end;
	int cnt = 0;
	
	while (ch != 0)
	{
		if (ch == token)
		{
			*end = '\0';
			argv[cnt++] = start;
			start = end + 1;
			end = start;
		}
		else
		{
			end++;
		}
		
		ch = *end;
	}
	
	argv[cnt] = start;
	return cnt + 1;
}

void cmdTblIterate(void (*callFunc)(void *))
{
    const struct cmd_table *cmdtp = NULL;
    
    for (cmdtp = &__cmd_start; cmdtp != &__cmd_end; cmdtp++)
    {
        callFunc((void *)cmdtp);
    }
}

/* find command table entry for a command */
struct cmd_table *find_cmd(const char *cmd, bool *isFullMatch)
{
	struct cmd_table *cmdtp;
	struct cmd_table *cmdtp_temp = &__cmd_start;
	size_t len;
	int found = 0;
	
	len = strlen(cmd);
	
	for (cmdtp = &__cmd_start; cmdtp != &__cmd_end; cmdtp++)
	{
		if (strncmp(cmd, cmdtp->name, (size_t)len) == 0)
		{
			if ((size_t)len == strlen(cmdtp->name))
			{
				if (isFullMatch != NULL)
				{
					*isFullMatch = true;
				}
				return cmdtp; /* full match */
			}

			cmdtp_temp = cmdtp; /* abbreviated command ? */
			found++;
		}
	}
	
	if (found == 1)
	{
		/* exactly one match */
		if (isFullMatch != NULL)
		{
			*isFullMatch = false;
		}
		return cmdtp_temp;
	}
	
	if (isFullMatch != NULL)
	{
		*isFullMatch = false;
	}
	return NULL;	/* not found or ambiguous command */
}

void shell_loop(void)
{
	char cmd_buf[CMD_BUF_MAX_SIZE+1] = {0};
	int len = 0;
	char *argv[CONFIG_SYS_MAXARGS + 1]; 	/* NULL terminated */
	int argc = 0;
	
	bool full_match = false;
	
	struct cmd_table *cmdtp = NULL;
	
	serial_puts(CONFIG_SYS_VERSION);
	
	while(1)
	{
		len = readline(CONFIG_SYS_PROMPT, cmd_buf, CMD_BUF_MAX_SIZE);
		if (len > 0)
		{
			/* extract arguments */
			argc = parse_line(cmd_buf, ' ', argv);
			
			/* look up command in command table */
			cmdtp = find_cmd(argv[0], &full_match);

            if (history_count > 31)
            {
                history_count = 0;
            }
            history_cmd[history_count++] = argv[0];
            
			if ((cmdtp == NULL) || (full_match == false))
			{
				printk("Unknown command '%s' - try 'help'\n", argv[0]);
				memset(cmd_buf, 0, CMD_BUF_MAX_SIZE + 1);
				continue;
			}
			
			/* found - check max args */
			if (argc > cmdtp->maxargs)
			{
				printk("Usage:\n%s\n", cmdtp->usage);
				memset(cmd_buf, 0, CMD_BUF_MAX_SIZE + 1);
				continue;
			}
			/* call function to do the command */
			if (cmdtp->cmd != NULL)
			{
				cmdtp->cmd(cmdtp, argc, argv);
			}
			else
			{
				printk("execute %s cmd not implemented\n", argv[0]);
				memset(cmd_buf, 0, CMD_BUF_MAX_SIZE + 1);
			}
		}
	}
	
}

#define CMD_STRING_MAX_LEN		79
#define CMD_HISTROY_NUMBER		8


