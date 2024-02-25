CC = i686-elf-gcc
CC_FLAGS = -g -m32 -ffreestanding -nostdlib -Wall -Wextra -Ikernel/include -Ikenrel/include/libc

LD = i686-elf-ld
LD_FLAGS = 

AS = i686-elf-as
AS_FLAGS =

OBJCOPY = i686-elf-objcopy

NASM_FLAGS = -Ikernel/

OUTPUT = MONNOS.bin
SYMBOLS = MONNOS.sym

all: builddir ${OUTPUT} clean_dev

build/monnos/debug/libmonnos.a: builddir
	cargo build --target-dir ./build

run: all
	scripts/multiboot/mkiso.sh ${OUTPUT}
	qemu-system-i386 -cdrom MONNOS.iso -serial stdio

debug_run: ${SYMBOLS} all
	bash -c "qemu-system-i386 -s -S -fda ${OUTPUT}&"

show:
	@echo ${C_SOURCES} ${CXX_SOURCES} ${NASM_SOURCES}

MONNOS: builddir ${OUTPUT}

${OUTPUT}: build/boot_multiboot.o kernel/kernel_entry.o build/monnos/debug/libmonnos.a
	@echo "LD: $@"
	@${LD} -T linker.ld ${LD_FLAGS} -o $@ -Ttext 0x1000 $^

${SYMBOLS}: ${C_OBJECTS} ${CXX_OBJECTS} ${NASM_OBJECTS}
	@echo "SYM: $^"
	@${LD} ${LD_FLAGS} -o $@ -Ttext 0x1000 $^
	@mv ${SYMBOLS} ${SYMBOLS}.elf
	@${OBJCOPY} ${SYMBOLS}.elf ${SYMBOLS}
	@rm ${SYMBOLS}.elf

build/boot_multiboot.o: builddir
	@echo "KENTRY NASM: $@"
	@nasm -felf32 kernel/boot_multiboot.asm -o build/boot_multiboot.o

builddir:
	@-mkdir build

%.c.o: %.c
	@echo "CC: $^"
	@${CC} ${CC_FLAGS} -c $^ -o $@

%.asm.o: %.asm	
	@echo "NASM: $^"
	@nasm $^ -f elf -o $@

clean: clean_dev
	@-rm -rf ${OUTPUT} ${SYMBOLS}

clean_dev:
	@-rm -rf bootloader.bin