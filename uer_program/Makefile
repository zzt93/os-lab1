# **** GNU make manual: http://www.gnu.org/software/make/manual/make.html ****

CC      = gcc
LD      = ld
CFLAGS  = -m32 -static -ggdb -MD -Wall -Werror -I./include -O0 \
		 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer
LDFLAGS = -melf_i386 -e main

CFILES  = $(shell find src/ -name "*.c")
SFILES  = $(shell find src/ -name "*.S")
OBJS    = $(CFILES:.c=.o) $(SFILES:.S=.o)


compile: $(OBJS)

$(OBJS): % : %.o
	$(LD) $(LDFLAGS) -o $@ $<
	objdump -D $@ > $@.txt
	readelf -a $@ > $@.txt

clean:
	rm -f $(OBJS) $(OBJS:.o=.d)
