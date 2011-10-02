TEXT_BASE = 0x30000000
############################
#
# 交叉工具相关设置
#
############################

CROSS_COMPILE = arm-elf-

AS	= $(CROSS_COMPILE)as
LD	= $(CROSS_COMPILE)ld
CC	= $(CROSS_COMPILE)gcc
CPP	= $(CC) -E
AR	= $(CROSS_COMPILE)ar
NM	= $(CROSS_COMPILE)nm
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

############################
#
# 编译器相关标志设置
#
############################

DBGFLAGS = -g #-DLWIP_DEBUG
OPTFLAGS = -O2 #-fomit-frame-pointer

CPPFLAGS =  -mlittle-endian -std=c99 $(DBGFLAGS) $(OPTFLAGS) -DTEXT_BASE=${TEXT_BASE}\
			-I$(TOPDIR) \
			-fno-builtin -ffreestanding -nostdinc -Wbad-function-cast

CFLAGS = $(CPPFLAGS) -W -Wall -Wpointer-arith -Wstrict-prototypes -pedantic

ASMFLAGS = -Wall $(CPPFLAGS) -D__ASSEMBLY__ 



#########################################################################

export	CROSS_COMPILE AS LD CC CPP AR NM STRIP OBJCOPY OBJDUMP MAKE

export	TEXT_BASE CPPFLAGS CFLAGS ASMFLAGS

#########################################################################

############################
#
# $(CURDIR)由make自动产生, make -C
#
############################

%.s: %.S
	$(CPP) $(AFLAGS) -o $@ $(CURDIR)/$<
%.o: %.s
	$(CC) $(AFLAGS) -c -o $@ $(CURDIR)/$<
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

