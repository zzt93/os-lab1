# **** GNU make manual: http://www.gnu.org/software/make/manual/make.html ****

CFLAGS  = -m32 -static -ggdb -MD -Wall -Werror -I../include -O0 \
		 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer -fno-align-functions -Wno-main

CFILES  = $(shell find ./ -maxdepth 1 -name "*.c")
SFILES  = $(shell find ./ -maxdepth 1 -name "*.S")
OBJS    = $(CFILES:.c=.o) $(SFILES:.S=.o)



user: systemcall start
	@cd args; make
	@cd no_args; make


systemcall:
	@cd sys_call; make

start:$(OBJS)
	
clean:
	rm -f $(OBJS)
	@cd args; make clean
	@cd no_args; make clean
	@cd sys_call; make clean

