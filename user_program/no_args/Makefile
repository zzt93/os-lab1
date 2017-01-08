# **** GNU make manual: http://www.gnu.org/software/make/manual/make.html ****

include ../args/Makefile

LDFLAGS = -melf_i386 -e _start2

LIB     = ../../src/user_c_lib/*.o ../../src/kernel/manager/error.o ../start2.o

compile: $(OBJS)
