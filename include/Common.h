#ifndef STARDUST_COMMON_H
#define STARDUST_COMMON_H

//
// system headers
//
#include <windows.h>

//
// stardust headers
//
#include <common/Native.h>
#include <common/Macros.h>
#include <common/Defs.h>
#include <core/Ldr.h>
#include <crypto/Hash.h>

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

    struct {

        //
        // Ntdll.dll
        //
        D_API( RtlAllocateHeap        )
        D_API( NtProtectVirtualMemory )

        //
        // kernel32.dll
        //
        D_API( LoadLibraryW )

        //
        // User32.dll
        //
        D_API( MessageBoxW )

    } Win32;

    struct {
        PVOID Ntdll;
        PVOID Kernel32;
        PVOID User32;
    } Modules;

} INSTANCE, *PINSTANCE;

EXTERN_C PVOID StRipStart();
EXTERN_C PVOID StRipEnd();

VOID Main(
    _In_ PVOID Param
);

#endif //STARDUST_COMMON_H
