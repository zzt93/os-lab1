CC      = gcc
LD      = ld
CFLAGS  = -m32 -static -ggdb -MD -Wall -Werror -I./include -O0 \
		 -fno-builtin -fno-stack-protector -fno-omit-frame-pointer -Wreturn-type -std=gnu99
ASFLAGS = -m32 -MD -I./include
LDFLAGS = -melf_i386
QEMU    = qemu-system-i386

CFILES  = $(shell find src/ -name "*.c")
SFILES  = $(shell find src/ -name "*.S")
OBJS    = $(CFILES:.c=.o) $(SFILES:.S=.o)

run: hardDisk
	$(QEMU) -no-shutdown -serial stdio harddisk/harddisk.img

OBJS: replaceBinary

replaceBinary: user_pro
	mv ./src/drivers/ramdisk/ram.c ram.c
	sed 's/\/\*\(user_program\/[^*]*\)\*\//tail -n+2 \1 | head -n-2/e' <ram.c >./src/drivers/ramdisk/ram.c

replaceBack:
	mv ram.c ./src/drivers/ramdisk/ram.c

debug: hardDisk
	mkdir -p ./test/debug
	objdump -D kernel > test/debug/code.txt	# disassemble result
	readelf -a kernel > test/debug/elf.txt		# obtain more information about the executable
	gnome-terminal -e "bash -c \"gdb -s kernel; exec bash\""
	$(QEMU) -serial stdio -s -S harddisk/harddisk.img

hardDisk: disk.img
	@cd harddisk; make run

disk.img: kernel
	@cd boot; make
	cat boot/bootblock kernel > disk.img

kernel: $(OBJS)
	$(LD) $(LDFLAGS) -e os_init -Ttext 0xC0100000 -o kernel $(OBJS)

harddisk.img: disk.img
	python harddisk/makeimg.py
	cat harddisk/harddisk disk.img > disk.img

-include $(OBJS:.o=.d)

clean:
	@cd boot; make clean
	@cd user_program; make clean
	rm -f kernel disk.img $(OBJS) $(OBJS:.o=.d)

rebuild: clean
	make disk.img

user_pro: lib_comp
	@cd user_program; make

lib_comp:
	@cd user_program; make systemcall

test: hardDisk
	mkdir -p test/logs/
	bash test/test_press_key.sh
	bash test/repeat.sh