HOST?=i686-elf
HOSTARCH!=scripts/target-triplet-to-arch.sh $(HOST)
SYSROOT?=$(PWD)/sysroot
CROSS_BIN_DIR?=/home/john/opt/cross/bin
CC:=$(CROSS_BIN_DIR)/$(HOST)-gcc --sysroot=$(SYSROOT)
CXX:=$(CROSS_BIN_DIR)/$(HOST)-g++
AS:=$(CROSS_BIN_DIR)/$(HOST)-as
AR:=$(CROSS_BIN_DIR)/$(HOST)-ar

DESTDIR?=$(SYSROOT)

$(info CC is $(CC))
$(info CXX is $(CXX))
$(info AS is $(AS))
$(info AR is $(AR))
$(info DESTDIR is $(DESTDIR))

CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=

PREFIX?=/usr
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=/boot
LIBDIR?=$(EXEC_PREFIX)/lib
INCLUDEDIR?=$(PREFIX)/include

ifneq ($(substr -elf,, $(HOST)), $(HOST))
	CC:=$(CC) -isystem=$(INCLUDEDIR)
endif

# kernel setup
KERNEL_CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra -fstack-protector-all
KERNEL_CPPFLAGS:=$(CPPFLAGS) -D__is_kernel -Ikernel/include
KERNEL_LDFLAGS:=$(LDFLAGS)
KERNEL_LIBS:=$(LIBS) -nostdlib -lk -lgcc

KERNEL_ARCHDIR=kernel/arch/$(HOSTARCH)
include $(KERNEL_ARCHDIR)/make.config

CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)

KERNEL_CFLAGS:=$(KERNEL_CFLAGS) $(KERNEL_ARCH_CFLAGS)
KERNEL_CPPFLAGS:=$(KERNEL_CPPFLAGS) $(KERNEL_ARCH_CPPFLAGS)
KERNEL_LDFLAGS:=$(KERNEL_LDFLAGS) $(KERNEL_ARCH_LDFLAGS)
KERNEL_LIBS:=$(KERNEL_LIBS) $(KERNEL_ARCH_LIBS)

KERNEL_SRC_FILES:=$(shell find kernel -type f -name "*.c" -a -! -path "*arch*")
$(info KERNEL_SRC_FILES = $(KERNEL_SRC_FILES))
KERNEL_SRC_OBJS:=$(patsubst %.c,%.o,$(KERNEL_SRC_FILES))
KERNEL_OBJS=$(KERNEL_ARCH_MULTIBOOT_OBJS) $(KERNEL_ARCHDIR)/crti.o $(CRTBEGIN_OBJ) $(KERNEL_ARCH_OBJS) $(KERNEL_SRC_OBJS) $(KERNEL_ARCH_INTERRUPT_C_OBJS) $(CRTEND_OBJ) $(KERNEL_ARCHDIR)/crtn.o
KERNEL_LINK_LIST:=$(KERNEL_LDFLAGS) $(KERNEL_OBJS) $(KERNEL_LIBS)

# libc / libk setup
LIBC_ARCHDIR=libc/arch/$(HOSTARCH)
include $(LIBC_ARCHDIR)/make.config

LIBC_CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra -fstack-protector-all
LIBC_CPPFLAGS:=$(CPPFLAGS) -D__is_libc -Ilibc/include
LIBK_CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra -fstack-protector-all
LIBK_CPPFLAGS:=$(CPPFLAGS) -D__is_libk -Ilibc/include

HOSTEDOBJS=$(ARCH_HOSTEDOBJS)
FREEOBJS=$(ARCH_FREEOBJS)  \
libc/stdio/printf.o \
libc/stdio/putchar.o \
libc/stdio/puts.o \
libc/stdlib/abort.o \
libc/string/memcmp.o \
libc/string/memcpy.o \
libc/string/memmove.o \
libc/string/memset.o \
libc/string/strlen.o \
libc/string/strcpy.o \
libc/ssp/ssp.o \

LIBC_OBJS=$(FREEOBJS) $(HOSTEDOBJS)

LIBK_OBJS=$(FREEOBJS:.o=.libk.o)

TARGET_LIBS=libk.a

.PHONY: all clean install-headers install-kernel-headers install-libc-headers install-kernel install-libs test todolist run
.SUFFIXES: .o .c .S


all: install-headers $(TARGET_LIBS) install-libs foxos.iso

$(KERNEL_SRC_OBJS): %.o: %.c Makefile
	$(CC) -MD -MP -c $< -o $@ -std=gnu17 $(KERNEL_CFLAGS) $(KERNEL_CPPFLAGS)

$(KERNEL_ARCH_C_OBJS): %.o: %.c Makefile
	$(CC) -MD -MP -c $< -o $@ -std=gnu17 $(KERNEL_CFLAGS) $(KERNEL_CPPFLAGS)

$(KERNEL_ARCH_ASM_OBJS): %.o: %.s Makefile
	$(CC) -MD -MP -c $< -o $@ $(KERNEL_CFLAGS) $(KERNEL_CPPFLAGS)

$(KERNEL_ARCH_MULTIBOOT_OBJS): %.o: %.s Makefile
	$(CC) -MD -MP -c $< -o $@ -std=gnu17 $(KERNEL_CFLAGS) $(KERNEL_CPPFLAGS)

$(KERNEL_ARCH_INTERRUPT_C_OBJS): %.o: %.c Makefile
	$(CC) -MD -MP -c $< -o $@ -std=gnu17 $(KERNEL_CFLAGS) $(KERNEL_CPPFLAGS) $(KERNEL_ARCH_INTERRUPT_CFLAGS)

foxos.kernel: $(KERNEL_OBJS) $(KERNEL_ARCHDIR)/linker.ld Makefile $(TARGET_LIBS)
	$(CC) -T $(KERNEL_ARCHDIR)/linker.ld -o $@ $(KERNEL_CFLAGS) $(KERNEL_LINK_LIST)
	grub-file --is-x86-multiboot foxos.kernel

$(LIBC_OBJS): %.o: %.c Makefile
	$(CC) -MD -MP -c $< -o $@ -std=gnu17 $(LIBC_CFLAGS) $(LIBC_CPPFLAGS)

$(LIBK_OBJS): %.libk.o: %.c Makefile
	$(CC) -MD -MP -c $< -o $@ -std=gnu17 $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

libc.a: $(LIBC_OBJS) Makefile
	$(AR) rcs $@ $^

libk.a: $(LIBK_OBJS) Makefile
	$(AR) rcs $@ $(LIBK_OBJS)

install: install-headers install-libs install-kernel

install-headers: install-libc-headers install-kernel-headers

install-kernel-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps kernel/include/. $(DESTDIR)$(INCLUDEDIR)/.

install-libc-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps libc/include/. $(DESTDIR)$(INCLUDEDIR)/.

install-kernel: foxos.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp foxos.kernel $(DESTDIR)$(BOOTDIR)

install-libs: $(TARGET_LIBS)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp $^ $(DESTDIR)$(LIBDIR)


clean:
	rm -rf foxos.iso foxos.kernel isodir sysroot $(KERNEL_ARCHDIR)/crti.o $(KERNEL_ARCH_OBJS) $(KERNEL_SRC_OBJS) $(KERNEL_ARCH_OBJS:.o=.d) $(KERNEL_SRC_OBJS:.o=.d) $(KERNEL_ARCHDIR)/crtn.o $(TARGET_LIBS) $(LIBC_OBJS) $(LIBC_OBJS:.o=.d) $(LIBK_OBJS) $(LIBK_OBJS:.libk.o=.libk.d)

foxos.iso: install-kernel 
	mkdir -p isodir/boot/grub
	cp sysroot/boot/foxos.kernel isodir/boot/foxos.kernel
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o foxos.iso isodir
	rm -rf isodir

todolist:
	-@rg -T make "TODO|FIXME"

run:
	@scripts/run_qemu.sh foxos.iso

-include $(KERNEL_OBJS:.o=.d)
-include $(LIBC_OBJS:.o=.d)
-include $(LIBK_OBJS:.o=.d)
