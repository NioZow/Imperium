#ifndef IMPERIUM_DEFS_H
#define IMPERIUM_DEFS_H

#include <imperium/native.h>

typedef struct _SYMBOL_HASH {
  ULONG Module;    // module hash
  ULONG Function;  // function hash
} SYMBOL_HASH, *PSYMBOL_HASH;

typedef struct _SYSCALL {
  PVOID  Address;
  USHORT Ssn;
} SYSCALL, *PSYSCALL;

//
// enum definition
//
typedef enum _WIN32_RESOLVE_FLAGS:
    ULONG {
      SymbolSyscall      = 0x01,
      SyscallAddInstance = 0x02,
    } WIN32_RESOLVE_FLAGS,
    *PWIN32_RESOLVE_FLAGS;

//
// structure definition
//
typedef struct _BUFFER {
  PVOID Buffer;
  ULONG Length;
} BUFFER, *PBUFFER;

typedef struct _SYMBOL {
  union {
    PVOID Address;

    SYSCALL Syscall;
  };

  ULONG           ModuleHash;
  ULONG           FunctionHash;
  struct _SYMBOL* Next;
} SYMBOL, *PSYMBOL;

typedef struct _INSTANCE {
  //
  // context to find our instance in memory
  //
  ULONG Context;

  //
  // syscall structure for the current syscall to be executed
  //
  PSYSCALL Syscall;

  //
  // store already loaded functions
  //
  PSYMBOL Symbol;

  //
  // base address and size
  // of the implant
  //
  BUFFER Base;

  HANDLE ConsoleOutput;
} INSTANCE, *PINSTANCE;

typedef PVOID( NTAPI* fnRtlAllocateHeap )( IN PVOID HeapHandle, IN OPTIONAL ULONG Flags, IN SIZE_T Size );

typedef PVOID(
    NTAPI* fnRtlReAllocateHeap )( IN PVOID HeapHandle, IN ULONG Flags, IN OPTIONAL PVOID BaseAddress, IN SIZE_T Size );

typedef PVOID( NTAPI* fnRtlFreeHeap )( IN PVOID HeapHandle, IN OPTIONAL ULONG Flags, IN OPTIONAL PVOID BaseAddress );

typedef BOOL( WINAPI* fnAttachConsole )( IN DWORD dwProcessId );

typedef HANDLE WINAPI( WINAPI* fnGetStdHandle )( IN DWORD nStdHandle );

typedef BOOL( WINAPI* fnWriteConsoleA )( IN HANDLE hConsoleOutput,
    IN const PVOID                                 lpBuffer,
    IN DWORD                                       nNumberOfCharsToWrite,
    OUT OPTIONAL LPDWORD                           lpNumberOfCharsWritten,
    LPVOID                                         lpReserved );

typedef int( WINAPI* fnVsnprintf )( char* buffer, size_t count, const char* format, va_list argptr );

typedef PVOID( WINAPI* fnLoadLibraryA )( PCSTR Module );

typedef INT( WINAPI* fnMessageBoxA )( IN OPTIONAL HWND hWnd,
    IN OPTIONAL LPCSTR                                 lpText,
    IN OPTIONAL LPCSTR                                 lpCaption,
    IN UINT                                            uType );

#endif  // IMPERIUM_DEFS_H
