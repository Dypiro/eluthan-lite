
CC = gcc
CFLAGS = -m32 -c -w -fno-stack-protector -ffreestanding -nostdlib
ASM = nasm
ASMFLAGS = -f elf32
LINK = ld
LINKFLAGS = -m elf_i386 -T 
MKDIR_P = mkdir -p
MV = mv
CP = cp
ISODIR = isodir
OUTDIR = bin

.PHONY: directories

all: directories kernel.bin forwardos.iso
	${MV} kernel.bin ${OUTDIR}

kernel.bin: start.o main.o system.o gdt.o idt.o isrs.o irq.o timer.o kb.o drive_mgr.o vfs.o Heap.o 
	${LINK} ${LINKFLAGS} src/link.ld -o kernel.bin src/start.o src/main.o src/system.o src/gdt.o src/idt.o src/isrs.o src/irq.o src/timer.o src/kb.o src/VFS/drive_mgr.o src/VFS/vfs.o src/Heap/Heap.o
	${CP} kernel.bin ${OUTDIR}
start.o: src/start.asm
	${ASM} ${ASMFLAGS} -o src/start.o src/start.asm

main.o: src/main.c
	${CC} ${CFLAGS} src/main.c -o src/main.o

system.o: src/system.c
	${CC} ${CFLAGS} src/system.c -o src/system.o

gdt.o: src/gdt.c
	${CC} ${CFLAGS} src/gdt.c -o src/gdt.o

idt.o: src/idt.c
	${CC} ${CFLAGS} src/idt.c -o src/idt.o

isrs.o: src/isrs.c
	${CC} ${CFLAGS} src/isrs.c -o src/isrs.o

irq.o: src/irq.o
	${CC} ${CFLAGS} src/irq.c -o src/irq.o

timer.o: src/timer.c
	${CC} ${CFLAGS} src/timer.c -o src/timer.o

kb.o: src/kb.c
	${CC} ${CFLAGS} src/kb.c -o src/kb.o
drive_mgr.o: src/VFS/drive_mgr.c
	${CC} ${CFLAGS} src/VFS/drive_mgr.c -o src/VFS/drive_mgr.o
vfs.o: src/VFS/vfs.c
	${CC} ${CFLAGS} src/VFS/vfs.c -o src/VFS/vfs.o
drive_mgr.o: src/VFS/drive_mgr.c
	${CC} ${CFLAGS} src/VFS/drive_mgr.c -o src/VFS/drive_mgr.o
Heap.o: src/Heap/Heap.c
	${CC} ${CFLAGS} src/Heap/Heap.c -o src/Heap/Heap.o

forwardos.iso: kernel.bin
	${MKDIR_P} ${ISODIR}
	${MKDIR_P} ${ISODIR}/boot
	${CP} ${OUTDIR}/kernel.bin ${ISODIR}/boot/kernel.bin
	${MKDIR_P} ${ISODIR}/boot/grub
	${CP} grub.cfg ${ISODIR}/boot/grub/grub.cfg
	grub-mkrescue -o eluthan-lite.iso ${ISODIR}

directories: ${OUTDIR}

${OUTDIR}:
	${MKDIR_P} ${OUTDIR}

run: bin/kernel.bin
	qemu-system-i386 -cdrom eluthan-lite.iso

clean:
	rm *.iso src/*.o
	rm -rf bin isodir