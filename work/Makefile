# Makefile 
# this file is for the develop for the GPIO
# that the file should be in the folder of reg
# set files
TARGET=ARM
CROSS=arm-hisiv100nptl-linux
CC  = $(CROSS)-gcc
STRIP=$(CROSS)-strip
#
# Configuration: Debug
#

TOPDIR=../
INCDIR=$(TOPDIR)include
SRCDIR=$(TOPDIR)source
OUTDIR=$(SRCDIR)

CFG_INC +=-I$(INCDIR)/common\
		  -I$(INCDIR)/utils\
		  -I$(INCDIR)/tools
HICOMMON_OBJ :=



#MAIN_OBJ=main.o 
MAIN_SRC :=$(wildcard *.c)
MAIN_OBJ :=$(MAIN_SRC:%.c=%.o)
#OUTTARGET :=$(MAIN_OBJ:%.o=%)
OUTTARGET := main
#!!!!!!!!!!!!!!! 1 添加生成的可执行文件
CFG_LIB = -lpthread -lrt

HICOMMON_OBJ=$(OUTDIR)/hi_message.o \
             $(OUTDIR)/hi_thread.o  \
             $(OUTDIR)/hi_dbg.o \
             $(OUTDIR)/cmdshell.o \
             $(OUTDIR)/argparser.o
BASE_OBJ=$(OUTDIR)/strfunc.o \
         $(OUTDIR)/memmap.o 

OBJ=$(HICOMMON_OBJ) \
    $(BASE_OBJ) 

ALL_OBJ = $(OBJ)\
		  $(MAIN_OBJ) \
        

CFG_FLAGS = $(CFG_DEBUG) $(CFG_INC) $(CFG_DEFS) 

COMPILE=$(CC) $(CFG_FLAGS) -c -o "$(OUTDIR)/$(*F).o" "$<"

all : $(OUTTARGET)
# complie the base api
$(OUTDIR)/%.o : $(SRCDIR)/utils/%.c
	$(COMPILE)
$(OUTDIR)/%.o : $(SRCDIR)/common/%.c
	$(COMPILE)
$(OUTDIR)/%.o :  $(DRIVERDIR)/memmap/%.c
	$(COMPILE)

$(MAIN_OBJ) :%.o:%.c $(OUTDIR) $(OBJ) 
		$(CC) $(CFG_CFLAGS)  $(CFG_INC) -lpthread -c $(MAIN_SRC) 

#$(OUTTARGET) :%:%.o $(BASE_OBJ) $(HICOMMON_OBJ)
$(OUTTARGET) :$(BASE_OBJ) $(HICOMMON_OBJ) $(MAIN_OBJ)
	#$(CC) $(CFG_INC) -lpthread -lm -o $@ $^
	$(CC) $(CFG_INC) -lpthread -lm -o $(OUTTARGET) $^

clean:
	rm -f $(OUTFILE)
	rm -f $(OBJ)
	rm -f $(OUTDIR)/*.o
	rm -rf $(MAIN_OBJ)
	rm -rf $(OUTTARGET)
	#rm -f himd himm himc himd.l hier hiew
	rm -f hiddrs hil2s

# Clean this project and all dependencies
cleanall: clean

