# **** GNU make manual: http://www.gnu.org/software/make/manual/make.html ****

CC      = gcc
LD      = ld
CFLAGS  = -m32 -static -ggdb -MD -Wall -Werror -I../include -O0 \
		 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer -fno-align-functions -Wno-main
LDFLAGS = -melf_i386 -e entry

CFILES  = $(shell find ./ -maxdepth 1 -name "*.c")
SFILES  = $(shell find ./ -maxdepth 1 -name "*.S")
LIB     = $(shell find sys_call/ lib/ -name "*.o")
OBJS    = $(CFILES:.c=.out) $(SFILES:.S=.out)
ARRAYS  = $(OBJS:.out=.ar)


compile: $(OBJS)

#xxd: $(ARRAYS)

$(OBJS): %.out : %.o
	$(LD) $(LDFLAGS) -o $@ $< $(LIB)
	objdump -D $@ > $(subst out,dump,$@)
	readelf -a $@ > $(subst out,elf,$@)
	xxd -c 4 -s 0 -i $@ > $(subst out,ar,$@)

lib_comp:
	@cd lib; make
	@cd sys_call; make

#$(ARRAYS): %.ar : %.out
#	xxd -c 4 -s 0 -i $< > $@

clean:
	rm -f $(OBJS) $(OBJS:.o=.d) $(OBJS:.c=.dump) $(OBJS:.c=.elf)
