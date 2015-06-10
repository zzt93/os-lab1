# **** GNU make manual: http://www.gnu.org/software/make/manual/make.html ****

CC      = gcc
LD      = ld
CFLAGS  = -m32 -static -ggdb -MD -Wall -Werror -I./include -O0 \
		 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer
LDFLAGS = -melf_i386 -e main

CFILES  = $(shell find ./ -name "*.c")
SFILES  = $(shell find ./ -name "*.S")
OBJS    = $(CFILES:.c=.out) $(SFILES:.S=.out)
ARRAYS  = $(OBJS:.out=.ar)


compile: $(OBJS)

#xxd: $(ARRAYS)

$(OBJS): %.out : %.o
	$(LD) $(LDFLAGS) -o $@ $<
	objdump -D $@ > $(subst out,dump,$@)
	readelf -a $@ > $(subst out,elf,$@)
	xxd -c 4 -s 0 -i $@ > $(subst out,ar,$@)

#$(ARRAYS): %.ar : %.out
#	xxd -c 4 -s 0 -i $< > $@	

clean:
	rm -f $(OBJS) $(OBJS:.o=.d) $(OBJS:.c=.dump) $(OBJS:.c=.elf)
