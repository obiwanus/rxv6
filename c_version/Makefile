# Build the kernel image by default
all: clean build/xv6.img

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

.PHONY:
clean:
	rm -rf build && mkdir -p build/kernel

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
	-Oz -Wall -ggdb -Werror -Iinclude -m32 \
	-std=c18
# -fno-pic 		= disable position-independent code
# -no-pie 		= disable position-independent executable
# -static		= link statically
# -fno-builtin	= don't recognise builtin functions
# -fno-omit-frame-pointer = don't optimise away rbp usage
# -fno-stack-protector = don't include code protecting from buffer overflow
# -nostdinc		= ignore standard include paths
# -Oz			= optimise aggressively for size

LDFLAGS = -m elf_i386
# (for the boot block we're also using -N):
# -N 			= 1) sets the text and data sections to be readable and writable
#				  2) disables page-aligning of the data segment
#				  3) disables linking against shared libraries

build/bootblock: boot/loader.asm boot/loader_main.c
	nasm -f elf32 -g boot/loader.asm -o build/loader.o
	$(CC) $(CFLAGS) -c boot/loader_main.c -o build/loader_main.o
	ld $(LDFLAGS) -N -e start -Ttext 0x7C00 build/loader.o build/loader_main.o -o build/bootblock.o
	objcopy -S -O binary -j .text build/bootblock.o build/bootblock
	./sign.py build/bootblock

KERNEL_OBJS = \
	build/kernel/memory.o \
	build/kernel/spinlock.o \
	build/kernel/mp.o \
	build/kernel/console.o \
	build/kernel/main.o

COMMON_OBJS = \
	build/string.o

# These .o files also depend on headers but we're always making a clean build
# so it's OK not to specify them here
build/kernel/%.o: kernel/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

build/%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

build/kernel_img: kernel/entry.asm kernel/linker-script.ld $(KERNEL_OBJS) $(COMMON_OBJS)
	nasm -f elf32 -g kernel/entry.asm -o build/entry.o
	ld $(LDFLAGS) -T kernel/linker-script.ld build/entry.o $(KERNEL_OBJS) $(COMMON_OBJS) -o build/kernel_img

build/xv6.img: build/bootblock build/kernel_img
	dd if=/dev/zero of=build/xv6.img count=10000
	dd if=build/bootblock of=build/xv6.img conv=notrunc
	dd if=build/kernel_img of=build/xv6.img seek=1 conv=notrunc
