MAKEFLAGS += "-s -j 16"

##
## Project name
##
Project := imperium

##
## Compilers
##
CXX := x86_64-w64-mingw32-g++

##
## Compiler flags
##
CFLAGS := -Os -fno-asynchronous-unwind-tables
CFLAGS += -fno-ident -fpack-struct=8 -falign-functions=1
CFLAGS += -s -ffunction-sections -falign-jumps=1 -w
CFLAGS += -falign-labels=1 -fPIC
CFLAGS += -fno-builtin
CFLAGS += -Iinclude -masm=intel -fpermissive -mrdrnd -std=c++20
CFLAGS += -fno-stack-protector -fno-exceptions -fno-rtti

##
## Source files
##
vpath %.cc src src/imperium src/imperium/entry
vpath %.x64.asm src/imperium src/imperium/entry

SRC := $(wildcard src/*.cc src/imperium/*.cc)
ASM := $(wildcard src/*.x64.asm src/imperium/*.x64.asm)

##
## PIC
##
SRC_PIC     := $(SRC) src/imperium/entry/pic.cc
ASM_PIC     := $(ASM) src/imperium/entry/entry.x64.asm
OBJ_PIC     := $(addprefix bin/obj/,$(notdir $(SRC_PIC:.cc=.o))) $(addprefix bin/obj/,$(notdir $(ASM_PIC:.asm=.o))) 
OUT_PIC     := bin/$(Project).x64.bin
CFLAGS_PIC  := $(CFLAGS) -DIMPERIUM_PIC
LDFLAGS_PIC := -Wl,-Tscripts/Linker.ld -Wl,-s,--no-seh,--enable-stdcall-fixup -nostdlib -nodefaultlibs 

##
## EXE
##
SRC_EXE     := $(SRC) src/imperium/entry/exe.cc
ASM_EXE     := $(ASM)
OBJ_EXE     := $(addprefix bin/obj/,$(notdir $(ASM_EXE:.asm=.o))) 
OUT_EXE     := bin/$(Project).x64.exe
CFLAGS_EXE  := $(CFLAGS) -DIMPERIUM_EXE
LDFLAGS_EXE :=

##
## DLL
##
SRC_DLL     := $(SRC) src/imperium/entry/dll.cc
ASM_DLL     := $(ASM)
OBJ_DLL     := $(addprefix bin/obj/,$(notdir $(ASM_DLL:.asm=.o))) 
OUT_DLL     := bin/$(Project).x64.dll
CFLAGS_DLL  := $(CFLAGS) -DIMPERIUM_DLL
LDFLAGS_DLL := -shared

##
## main target
##
all: pic exe dll

##
## Build stardust source into an
## executable and extract shellcode
##
pic: $(OBJ_PIC)
	@ echo "-> linking x64 shellcode"
	@ $(CXX) $(OBJ_PIC) -o $(OUT_PIC) $(LDFLAGS_PIC) $(CFLAGS_PIC)
	@ scripts/build.py -f $(OUT_PIC) -o $(OUT_PIC)

exe: $(OBJ_EXE)
	@ echo "-> compiling x64 exe"
	@ $(CXX) $(SRC_EXE) $(OBJ_EXE) -o $(OUT_EXE) $(LDFLAGS_EXE) $(CFLAGS_EXE)

dll: $(OBJ_DLL)
	@ echo "-> compiling x64 dll"
	@ $(CXX) $(SRC_DLL) $(OBJ_DLL) -o $(OUT_DLL) $(LDFLAGS_DLL) $(CFLAGS_DLL)

ldr:
	@ echo "-> compiling loader"
	@ $(CXX) scripts/loader.c -o bin/loader.x64.exe

##
## build source to object files
##
bin/obj/%.o: %.cc
	@ echo "-> compiling $< to $(notdir $@)"
	@ $(CXX) -o $@ -c $< $(CFLAGS_PIC)

bin/obj/%.o: %.asm
	@ echo "-> compiling $< to $(notdir $@)"
	@ nasm -f win64 -o $@ $<

##
## Clean object files and other binaries
##
clean:
	@ rm -rf bin/*.bin
	@ rm -rf bin/*.exe
	@ rm -rf bin/*.dll
	@ rm -rf bin/*.o
	@ rm -rf bin/obj/*.o
	@ rm -rf bin/obj/pic/*.o
	@ rm -rf bin/obj/exe/*.o
	@ rm -rf bin/obj/dll/*.o


