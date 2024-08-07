cmake_minimum_required(VERSION 3.26)
project(project-template LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_CFLAGS "-fpermissive")

include_directories(include)


add_compile_definitions(INDIRECT_SYSCALL)

add_executable(project-template src/main/Main.cc
        src/main/PreMain.cc
        src/core/Syscall.cc
        src/core/Ldr.cc
        src/core/Utils.cc
        src/core/Runtime.cc
        src/core/Win32.cc
        src/asm/x64/Stardust.asm
        src/asm/x64/Syscall.asm
        include/Common.h
        include/common/Imperium.h
        include/common/Native.h
        include/common/Stardust.h
        include/core/Ldr.h
        include/core/Syscall.h
        include/core/Utils.h
        include/core/Win32.h
        include/core/Runtime.h
        include/crypto/Hash.h
        src/common/Imperium.cc
        src/crypto/Hash.cc
        include/common/Common.h
)
