KERNEL := kernel.elf

CC = x86_64-elf-gcc
LD = x86_64-elf-ld
NASM = nasm

PWD = $(shell pwd)

CFLAGS = -g -Wall -Wextra -O2 -pipe
NASMFLAGS = 
LDFLAGS =

# internal c flags that shouldn't be changed by the user.
INTERNALCFLAGS :=        \
	-I.                  \
	-I$(PWD)/src         \
	-std=gnu11           \
	-ffreestanding       \
	-fno-stack-protector \
	-fno-pic -fPIE       \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone

# Internal linker flags that should not be changed by the user.
INTERNALLDFLAGS :=             \
	-Tlinker.ld            \
	-nostdlib              \
	-zmax-page-size=0x1000 \
	-static                \
	-pie                   \
	--no-dynamic-linker    \
	-ztext

C_SRCS := $(wildcard src/*.c) $(wildcard src/klib/*.c) $(wildcard src/cpu/*.c) $(wildcard src/console/*.c) $(wildcard src/kernel/*.c) $(wildcard src/mm/*.c)
AS_SRCS := src/cpu/idt.s src/cpu/interrupt.s
OBJS := $(C_SRCS:.c=.o) $(AS_SRCS:.s=.s.o)

.PHONY: all clean print

all: $(KERNEL)

print:
	echo $(OBJS)

$(KERNEL): $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) $(INTERNALLDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $< -o $@

%.s.o: %.s
	$(NASM) -f elf64 $(NASMFLAGS) $< -o $@ 

iso: $(KERNEL)
	rm -rf iso_root
	mkdir -p iso_root
	cp -v $(KERNEL) limine.cfg limine/limine.sys \
		limine/limine-cd.bin limine/limine-eltorito-efi.bin iso_root/
	
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-eltorito-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o image.iso
	
	./limine/limine-install image.iso

clean:
	rm -rf $(KERNEL) $(OBJS) iso_root image.iso

