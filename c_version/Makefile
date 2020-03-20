QEMU = qemu-system-x86_64
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
	-O2 -Wall -ggdb -Werror -I.
# -fno-pic 		= disable position-independent code
# -no-pie 		= disable position-independent executable
# -static		= link statically
# -fno-builtin	= don't recognise builtin functions
# -fno-omit-frame-pointer = don't optimise away rbp usage
# -fno-stack-protector = don't include code protecting from buffer overflow
# -nostdinc		= ignore standard include paths

$(shell rm -rf build && mkdir -p build)

build/bootblock: boot/loader.asm boot/loader_main.c
	nasm -f elf64 -g boot/loader.asm -o build/loader.o
	$(CC) $(CFLAGS) -c boot/loader_main.c -o build/loader_main.o
	ld -N -e start -Ttext 0x7C00 build/loader.o build/loader_main.o -o build/bootblock

