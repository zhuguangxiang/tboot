#ifndef _COMMAND_H_
#define _COMMAND_H_
struct cmd_table 
{
	char *name;   /* Command Name	*/
	int maxargs;  /* maximum number of arguments */
	
	/* Implementation function */
    int (*cmd)(struct cmd_table *table, int argc, char *argv[]);
	
	char *usage; /* Usage message (short) */
	char *help;  /* Help message (long) */
};

#define CMD_SECTION_ATTR  __attribute__ ((unused,section (".cmd_tbl_item")))

#define TBOOT_CMD(name,maxargs,cmd,usage,help) \
struct cmd_table  __cmd_##name CMD_SECTION_ATTR = {#name,maxargs,cmd,usage, help}

#define CONFIG_SYS_VERSION	"TinyBoot Version 0.01 By zgx\n"
#define CONFIG_SYS_PROMPT	"TBoot>"
#define CONFIG_SYS_MAXARGS	8

#define CMD_BUF_MAX_SIZE	511

#endif
