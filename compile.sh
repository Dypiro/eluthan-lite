cd src/bootloader/stage1
make clean
make
cd ..
cd stage2
make clean
make
cd ../../
cd kernel
make
cd ../..
make clean
make
gcc -g -o build/tools/fat tools/fat/fat.c