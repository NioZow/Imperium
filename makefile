MAKEFLAGS += "-s -j 16"

##
## Project name
##
Project := imperium

##
## Compilers
##
CC_X64 := x86_64-w64-mingw32-g++
CC_X86 := clang++ -target i686-w64-mingw32
CC_ASM := nasm

##
## Compiler flags
##
CFLAGS := -Os -fno-asynchronous-unwind-tables
CFLAGS += -fno-ident -fpack-struct=8 -falign-functions=1
CFLAGS += -s -ffunction-sections -falign-jumps=1 -w
CFLAGS += -falign-labels=1 -fPIC
CFLAGS += -Wl,-s,--no-seh,--enable-stdcall-fixup -fno-builtin-memset
CFLAGS += -Iinclude -masm=intel -fpermissive -mrdrnd -std=c++20 ${DEFINES}
CFLAGS += -fno-stack-protector -fno-exceptions -fno-rtti -nostdlib -nodefaultlibs

LDFLAGS := -Wl,-Tscripts/Linker.ld

SHE_FLAGS := -DIMPERIUM_SHELLCODE
EXE_FLAGS := -DIMPERIUM_EXE
DLL_FLAGS := -DIMPERIUM_DLL

##
## Stardust source and object files
##
SRC_CC := $(wildcard src/*.cc src/imperium/*.cc src/imperium/entry/shellcode.cc)
SRC_ASM := $(wildcard src/asm/*.x64.asm)

# Use vpath to tell make where to find source files
vpath %.cc src src/imperium src/imperium/entry
vpath %.x64.asm src/asm

OBJ_CC := $(addprefix bin/obj/,$(notdir $(SRC_CC:.cc=.x64.o)))
OBJ_ASM := $(addprefix bin/obj/,$(notdir $(SRC_ASM:.x64.asm=.x64.o)))
OBJ_X64 := $(OBJ_CC) $(OBJ_ASM)
OBJ_X64_EXE := $(filter-out bin/obj/entry.x64.o,$(OBJ_ASM))

##
## x64 binaries
##
exe-x64 := bin/$(Project).x64.exe
sh-x64 := bin/$(Project).x64.bin
dll-x64 := bin/$(Project).x64.dll

##
## main target
##
all: shellcode exe

##
## Build stardust source into an
## executable and extract shellcode
##
shellcode: $(OBJ_X64)
	@ echo "-> linking x64 shellcode"
	@ $(CC_X64) bin/obj/*.x64.o -o $(exe-x64) $(CFLAGS) $(LDFLAGS) $(SHE_FLAGS)
	@ scripts/build.py -f $(exe-x64) -o $(sh-x64)
	@ rm $(exe-x64)

##
## Build source to object files
## For shellcode
##
bin/obj/%.x64.o: %.cc
	@ echo "-> compiling $< to $(notdir $@)"
	@ $(CC_X64) -o $@ -c $< $(CFLAGS) $(SHE_FLAGS)

bin/obj/%.x64.o: %.x64.asm
	@ echo "-> compiling $< to $(notdir $@)"
	@ nasm -f win64 -o $@ $<

exe: $(OBJ_X64_EXE)
	@ echo "-> compiling x64 exe"
	@ $(CC_X64) $(SRC_CC) bin/obj/syscall.x64.o -o $(exe-x64) $(CFLAGS) $(EXE_FLAGS)

##
## build the loader
##
loader:
	@ echo "[*] compiling loader"
	@ x86_64-w64-mingw32-gcc scripts/loader.c -o bin/loader.x64.exe

##
## Clean object files and other binaries
##
clean:
	@ rm -rf bin/obj/*.o
	@ rm -rf bin/*.bin
	@ rm -rf bin/*.exe

