# Eluthan OS lite edition
A simple open-source terminal-based operating system that runs in grub.
Inspired from Forward OS
## Installation
```bash
$ sudo apt-get install xorriso build-essential qemu nasm 
$ make
```
make sure to use the cross compiler or replace CC = gcc and add -m32 to CCFLAGS to use normal gcc
## Usage
```bash
$ qemu-system-i386 -cdrom eluthan-lite.iso
```
precompiled iso
