#ifndef STARDUST_COMMON_H
#define STARDUST_COMMON_H

//
// system headers
//
#include <windows.h>
#include <stdio.h>

//
// stardust headers
//
#include <common/Native.h>
#include <common/Defs.h>
#include <common/Stardust.h>
#include <core/Win32.h>
#include <core/Utils.h>
#include <core/Ldr.h>
#include <crypto/Hash.h>
#include <core/Syscall.h>

//
// stardust instances
//
EXTERN_C ULONG __Instance_offset;
EXTERN_C PVOID __Instance;

typedef struct _INSTANCE {
    //
    // base address and size
    // of the implant
    //
    BUFFER Base;

    HANDLE ConsoleOutput;

    struct {
        //
        // ntdll.dll
        //
        D_API( RtlAllocateHeap );
        D_API( RtlReAllocateHeap );
        D_API( RtlCreateHeap );
        D_API( RtlFreeHeap );
        D_API( NtProtectVirtualMemory );
        //SYSCALL_INIT( NtProtectVirtualMemory );

        //
        // kernel32.dll
        //
        D_API( LoadLibraryW );
        D_API( AttachConsole );
        D_API( GetStdHandle );
        D_API( WriteConsoleA );

        //
        // user32.dll
        //
        D_API( MessageBoxW );

        //
        // msvcrt.dll
        //
        D_API( vsnprintf );
    } Win32;

    struct {
        PVOID Ntdll;
        PVOID Kernel32;
        PVOID User32;
        PVOID Msvcrt;
    } Modules;
} INSTANCE, *PINSTANCE;

EXTERN_C PVOID StRipStart();

EXTERN_C PVOID StRipEnd();

VOID Main(
    IN PVOID Param
);

enum RESOLVE_API_ERROR : CHAR {
    RESOLVE_API_SUCCESS               = 0,
    RESOLVE_API_ERROR_MODULE_NTDLL    = -1,
    RESOLVE_API_ERROR_MODULE_KERNEL32 = -2,
    RESOLVE_API_ERROR_MODULE_USER32   = -3,
    RESOLVE_API_ERROR_MODULE_MSVCRT   = -4,
};

#endif //STARDUST_COMMON_H
