#ifndef COMMON_COMMON_H
#define COMMMON_COMMON_H

#include <windows.h>
#include <common/Native.h>

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

typedef PVOID ( WINAPI*fnLoadLibraryA )(
    PCSTR Module
);

typedef INT ( WINAPI*fnMessageBoxA )(
    IN OPTIONAL HWND   hWnd,
    IN OPTIONAL LPCSTR lpText,
    IN OPTIONAL LPCSTR lpCaption,
    IN UINT            uType
);

typedef NTSTATUS ( NTAPI*fnNtOpenSection )(
    OUT PHANDLE           SectionHandle,
    IN ACCESS_MASK        DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
);

typedef NTSTATUS ( NTAPI*fnNtMapViewOfSection )(
    IN HANDLE                      SectionHandle,
    IN HANDLE                      ProcessHandle,
    IN OUT PVOID *                 BaseAddress,
    IN ULONG_PTR                   ZeroBits,
    IN SIZE_T                      CommitSize,
    IN OUT OPTIONAL PLARGE_INTEGER SectionOffset,
    IN OUT PSIZE_T                 ViewSize,
    IN SECTION_INHERIT             InheritDisposition,
    IN ULONG                       AllocationType,
    IN ULONG                       Win32Protect
);

typedef NTSTATUS ( NTAPI*fnNtProtectVirtualMemory )(
    IN HANDLE      ProcessHandle,
    IN OUT PVOID * BaseAddress,
    IN OUT PSIZE_T RegionSize,
    IN ULONG       NewProtect,
    OUT PULONG     OldProtect
);

#endif //COMMON_COMMON_H
