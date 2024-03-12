# Eluthan-os
A simple open-source terminal-based operating system that runs in grub.
Inspired from Forward OS
## Installation
```bash
$ sudo apt-get install xorriso build-essential qemu nasm 
$ make
```
## Usage
```bash
$ qemu-system-i386 -cdrom eluthan-lite.iso
```
there is already a premade iso in case you can't compile it