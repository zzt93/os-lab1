# **** GNU make manual: http://www.gnu.org/software/make/manual/make.html ****

CC      = gcc
LD      = ld
CFLAGS  = -m32 -static -ggdb -MD -Wall -Werror -I../../include -I../ -O0 \
		 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer -fno-align-functions -Wno-main
LDFLAGS = -melf_i386 -e _start2

CFILES  = $(shell find ./ -maxdepth 1 -name "*.c")
SFILES  = $(shell find ./ -maxdepth 1 -name "*.S")
SYS		= $(shell find ../sys_call/ -name "*.o")
LIB     = ../../src/lib/string_out.o ../start2.o
OBJS    = $(CFILES:.c=.out) $(SFILES:.S=.out)
ARRAYS  = $(OBJS:.out=.ar)


compile: $(OBJS)

#xxd: $(ARRAYS)

$(OBJS): %.out : %.o
	$(LD) $(LDFLAGS) -o $@ $< $(LIB) $(SYS)
	objdump -D $@ > $(subst out,dump,$@)
	readelf -a $@ > $(subst out,elf,$@)
	xxd -c 4 -s 0 -i $@ > $(subst out,ar,$@)


clean:
	rm -f $(OBJS) $(CFILES:.c=.o)
