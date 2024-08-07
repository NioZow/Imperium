MAKEFLAGS += "-s -j 16"

##
## Project name
##
Project := project-template

##
## Compilers
##
CC_X64	:= x86_64-w64-mingw32-g++

##
## Compiler flags
##
CFLAGS  := -Os -fno-asynchronous-unwind-tables -nostdlib
CFLAGS  += -fno-ident -fpack-struct=8 -falign-functions=1
CFLAGS  += -s -ffunction-sections -falign-jumps=1 -w
CFLAGS  += -falign-labels=1 -fPIC -Wl,-Tscripts/Linker.ld
CFLAGS  += -Wl,-s,--no-seh,--enable-stdcall-fixup
CFLAGS  += -Iinclude -masm=intel -fpermissive -mrdrnd -std=c++20 -DINDIRECT_SYSCALL

##
## Stardust source and object files
##
STAR-SRC := $(wildcard src/core/*.cc) $(wildcard src/crypto/*.cc) $(wildcard src/main/*.cc) $(wildcard src/common/*.cc)
STAR-OBJ := $(STAR-SRC:%.cc=%.o)

##
## x64 binaries
##
EXE-X64	:= bin/$(Project).x64.exe
BIN-X64	:= bin/$(Project).x64.bin

##
## main target
##
all: x64

##
## Build stardust source into an
## executable and extract shellcode
##
x64: clean asm-x64 $(STAR-OBJ)
	@ echo "[+] compile x64 executable"
	@ $(CC_X64) bin/obj/*.x64.o -o $(EXE-X64) $(CFLAGS)
	@ python3 scripts/build.py -f $(EXE-X64) -o $(BIN-X64)
	@ rm $(EXE-X64)

##
## Build source to object files
##
$(STAR-OBJ):
	@ $(CC_X64) -o bin/obj/$(Project)_$(basename $(notdir $@)).x64.o -c $(basename $@).cc $(CFLAGS)

##
## Build assemlby source to object files
##
asm-x64:
	@ echo "[*] compile assembly files"
	@ nasm -f win64 src/asm/x64/Stardust.asm -o bin/obj/asm_Stardust.x64.o
	@ nasm -f win64 src/asm/x64/Syscall.asm -o bin/obj/asm_Syscall.x64.o

##
## build the loader
##
loader:
	@ echo "[*] compiling loader"
	@ x86_64-w64-mingw32-gcc scripts/loader.c -o scripts/loader.x64.exe

##
## Clean object files and other binaries
##
clean:
	@ rm -rf bin/obj/*.o
	@ rm -rf bin/*.bin
	@ rm -rf bin/*.exe