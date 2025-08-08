MAKEFLAGS += "-s -j 16"

##
## project name
##
PROJECT := imperium

##
## compiler
##
CXX := x86_64-w64-mingw32-g++

##
## compiler flags
##
CFLAGS := -fno-asynchronous-unwind-tables                       # no stack unwinding
CFLAGS += -fpack-struct=8                                       # struct alignment
CFLAGS += -falign-functions=1 -falign-labels=1  -falign-jumps=1 # prevents aligning functions, jumps and labels
CFLAGS += -ffunction-sections                                   # put each function in a different section, helps elimate dead code
CFLAGS += -fPIC                                                 # position independant code
CFLAGS += -fno-builtin                                          # no builtin function such as memset, memcpy, etc.
CFLAGS += -fno-stack-protector                                  # disable stack canaries
CFLAGS += -fno-exceptions                                       # disable c++ exception handling
CFLAGS += -fno-rtti                                             # disable runtime type information
CFLAGS += -mrdrnd                                               # enable RBRAND cpu instrcution
CFLAGS += -Os                                                   # optimise for size, causes "redundant segment overrides" warnings but if not set shellcode does not work
CFLAGS += -Iinclude -masm=intel -std=c++20                      # use C++ 20, intel assembly and set the `include` to the include path

CFLAGS_RELEASE += -s                                            # strip all symbols (no function/variable names)
CFLAGS_RELEASE += -fno-ident                                    # no .ident section (which contains a string for indentifying compiler and its version)
CFLAGS_RELEASE += -DIMPERIUM_RELEASE

##
## source files
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
OUT_PIC     := bin/$(PROJECT).x64.bin
CFLAGS_PIC  := $(CFLAGS) -DIMPERIUM_PIC
LDFLAGS_PIC := -Wl,-Tscripts/Linker.ld -Wl,-s,--no-seh,--enable-stdcall-fixup -nostdlib -nodefaultlibs 

##
## EXE
##
SRC_EXE     := $(SRC) src/imperium/entry/exe.cc
ASM_EXE     := $(ASM)
OBJ_EXE     := $(addprefix bin/obj/,$(notdir $(ASM_EXE:.asm=.o))) 
OUT_EXE     := bin/$(PROJECT).x64.exe
CFLAGS_EXE  := $(CFLAGS) -DIMPERIUM_EXE
LDFLAGS_EXE :=

##
## DLL
##
SRC_DLL     := $(SRC) src/imperium/entry/dll.cc
ASM_DLL     := $(ASM)
OBJ_DLL     := $(addprefix bin/obj/,$(notdir $(ASM_DLL:.asm=.o))) 
OUT_DLL     := bin/$(PROJECT).x64.dll
CFLAGS_DLL  := $(CFLAGS) -DIMPERIUM_DLL
LDFLAGS_DLL := -shared

##
## targets
##
all: release
debug: pic exe dll
memleak: exe-memleak
release: pic-release exe-release dll-release

pic-release: CFLAGS_PIC += $(CFLAGS_RELEASE)
pic-release: pic

dll-release: CFLAGS_DLL += $(CFLAGS_RELEASE)  
dll-release: dll

exe-release: CFLAGS_EXE += $(CFLAGS_RELEASE) 
exe-release: exe

exe-memleak: CFLAGS_EXE += -DIMPERIUM_MEMLEAK
exe-memleak: exe

##
## build stardust source into an
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
## clean object files and other binaries
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


