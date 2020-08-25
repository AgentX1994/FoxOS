CROSS_BIN_DIR=/home/john/opt/cross/bin
CC=$(CROSS_BIN_DIR)/i686-elf-gcc
AS=$(CROSS_BIN_DIR)/i686-elf-as

FLAGS=-ffreestanding -O2 -Wall -Wextra -g
LD_FLAGS=-T linker.ld -ffreestanding -O2 -nostdlib -lgcc -g

all: myos.iso

boot.o: boot.s
	$(AS) $< -o $@

kernel.o: kernel.c
	$(CC) -c $< -o $@ $(FLAGS)

crti.o: crti.s
	$(AS) $< -o $@

crtn.o: crtn.s
	$(AS) $< -o $@

CRTBEGIN_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
OBJ_LINK_LIST:=crti.o $(CRTBEGIN_OBJ) kernel.o boot.o $(CRTEND_OBJ) crtn.o
INTERNAL_OBJS:=crti.o kernel.o boot.o crtn.o

myos.bin: $(OBJ_LINK_LIST)
	$(CC) $^ $(LD_FLAGS) -o $@

clean:
	rm -rf myos.bin $(INTERNAL_OBJS) isodir myos.iso

myos.iso: myos.bin
	mkdir -p isodir/boot/grub
	cp myos.bin isodir/boot/myos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir
	rm -rf isodir

