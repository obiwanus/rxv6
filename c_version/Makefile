# ============================================================
# ======================== EMULATION =========================
CPUS = 2
QEMU = qemu-system-i386
QEMUOPTS = \
	-drive file=build/xv6.img,index=0,media=disk,format=raw \
	-smp $(CPUS) \
	-m 512
QEMUGDB = -gdb tcp::251000	# port must match that in .gdbinit

qemu-gdb: build/xv6.img
	@echo "*** Now run gdb in another tab ***" 1>&2
	$(QEMU) -nographic $(QEMUOPTS) -S $(QEMUGDB)


# ============================================================
# ========================== BUILD ===========================

# Always make a clean build
$(shell rm -rf build && mkdir -p build)

CC = clang-9
CFLAGS = \
	-fno-pic \
	-fno-pie \
	-static \
	-fno-builtin \
	-fno-strict-aliasing \
	-fno-omit-frame-pointer  \
	-fno-stack-protector  \
	-nostdinc \
	-O2 -Wall -ggdb -Werror -Iinclude -m32
# -fno-pic 		= disable position-independent code
# -no-pie 		= disable position-independent executable
# -static		= link statically
# -fno-builtin	= don't recognise builtin functions
# -fno-omit-frame-pointer = don't optimise away rbp usage
# -fno-stack-protector = don't include code protecting from buffer overflow
# -nostdinc		= ignore standard include paths

build/bootblock: boot/loader.asm boot/loader_main.c
	nasm -f elf32 -g boot/loader.asm -o build/loader.o
	$(CC) $(CFLAGS) -c boot/loader_main.c -o build/loader_main.o
	ld -m elf_i386 -N -e start -Ttext 0x7C00 build/loader.o build/loader_main.o -o build/bootblock.o
	objcopy -S -O binary -j .text build/bootblock.o build/bootblock
	./sign.pl build/bootblock

build/xv6.img: build/bootblock
	dd if=/dev/zero of=build/xv6.img count=10000
	dd if=build/bootblock of=build/xv6.img conv=notrunc
