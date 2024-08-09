#ifndef COMMON_COMMON_H
#define COMMMON_COMMON_H

#include <windows.h>
#include <common/Native.h>

typedef PVOID ( WINAPI*fnGetModuleHandleA )(
    PSTR Module
);

typedef PVOID ( NTAPI*fnRtlAllocateHeap )(
    IN PVOID          HeapHandle,
    IN OPTIONAL ULONG Flags,
    IN SIZE_T         Size
);

typedef PVOID ( NTAPI*fnRtlReAllocateHeap )(
    IN PVOID          HeapHandle,
    IN ULONG          Flags,
    IN OPTIONAL PVOID BaseAddress,
    IN SIZE_T         Size
);

typedef PVOID ( NTAPI*fnRtlFreeHeap )(
    IN PVOID          HeapHandle,
    IN OPTIONAL ULONG Flags,
    IN OPTIONAL PVOID BaseAddress
);

typedef NTSTATUS ( NTAPI*fnNtProtectVirtualMemory )(
    IN HANDLE      ProcessHandle,
    IN OUT PVOID * BaseAddress,
    IN OUT PSIZE_T RegionSize,
    IN ULONG       NewProtect,
    OUT PULONG     OldProtect
);

typedef BOOL ( WINAPI*fnAttachConsole )(
    IN DWORD dwProcessId
);

typedef HANDLE WINAPI ( WINAPI*fnGetStdHandle )(
    IN DWORD nStdHandle
);

typedef BOOL ( WINAPI*fnWriteConsoleA )(
    IN HANDLE            hConsoleOutput,
    IN const PVOID       lpBuffer,
    IN DWORD             nNumberOfCharsToWrite,
    OUT OPTIONAL LPDWORD lpNumberOfCharsWritten,
    LPVOID               lpReserved
);

typedef int ( WINAPI*fnVsnprintf )(
    char *      buffer,
    size_t      count,
    const char *format,
    va_list     argptr
);

typedef NTSTATUS ( NTAPI*fnNtCreateFile )(
    OUT PHANDLE                FileHandle,
    IN ACCESS_MASK             DesiredAccess,
    IN POBJECT_ATTRIBUTES      ObjectAttributes,
    OUT PIO_STATUS_BLOCK       IoStatusBlock,
    IN OPTIONAL PLARGE_INTEGER AllocationSize,
    IN ULONG                   FileAttributes,
    IN ULONG                   ShareAccess,
    IN ULONG                   CreateDisposition,
    IN ULONG                   CreateOptions,
    IN OPTIONAL PVOID          EaBuffer,
    IN ULONG                   EaLength
);

typedef PVOID ( WINAPI*fnLoadLibraryA )(
    PCSTR Module
);

typedef INT ( WINAPI*fnMessageBoxA )(
    IN OPTIONAL HWND   hWnd,
    IN OPTIONAL LPCSTR lpText,
    IN OPTIONAL LPCSTR lpCaption,
    IN UINT            uType
);

#endif //COMMON_COMMON_H
