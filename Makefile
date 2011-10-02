#####################################

HOSTOS := $(shell uname -s | tr '[:upper:]' '[:lower:]')

ifeq ($(findstring mingw, $(HOSTOS)),mingw)
	TOPDIR = $(shell pwd | sed 's/^\///' | sed 's/^[a-zA-Z]*/&:/')
else
	ifeq ($(findstring cygwin, $(HOSTOS)),cygwin)
		TOPDIR = $(shell pwd)
	else
		TOPDIR = $(shell echo "no-any-host-support-by-this makefile")
	endif
endif

export	TOPDIR

LDSCRIPT = $(TOPDIR)/mini2440_boot.lds

LDFLAGS = -Bstatic -nostdlib -T $(LDSCRIPT) -Ttext $(TEXT_BASE)

include $(TOPDIR)/config.mk

SRC_DIR = 
OBJ_DIR = 
EXE_DIR = 
EXE_NAME = tinyboot.elf



######################################


OBJS = _divsi3.o  _modsi3.o  _udivsi3.o _umodsi3.o div0.o boot.o uart.o timer.o vsprintf.o ctype.o string.o command.o printk.o \
		xmodem.o xmodem_cmd.o crc16.o sst39vf1601.o

EXE_FILE = $(EXE_NAME)


LIBS = 
LIBS +=

SUBDIRS =



######################################

all: $(OBJS) $(OBJS2) 
	$(CC) $(OBJS) -o $(EXE_FILE)  $(LDFLAGS)
dump: all
	$(OBJDUMP) -D $(EXE_FILE) > $(EXE_FILE).dump
map: all
	$(NM) -v -l $(EXE_FILE) > $(EXE_FILE).map
bin: dump
	$(OBJCOPY) -O binary $(EXE_FILE) $(EXE_FILE).bin
clean:
	$(RM) -rf $(OBJS) $(OBJS2) $(EXE_FILE) $(EXE_FILE).dump $(EXE_FILE).map $(EXE_FILE).bin
	