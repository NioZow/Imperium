#ifndef CORE_SYSCALL_H
#define CORE_SYSCALL_H

#include <windows.h>

#ifdef INDIRECT_SYSCALL

typedef struct _SYSCALL {
  PVOID  Address;
  USHORT Ssn;
} SYSCALL, *PSYSCALL; 

EXTERN_C VOID SyscallPrepare(
  _In_ PSYSCALL Config
);

EXTERN_C NTSTATUS SyscallInvoke(
  _Inout_ ... // Args
);

#define SYSCALL_INIT( x )               SYSCALL * x;
#define SYSCALL_RESOLVE( Hash )         SyscallResolve( Hash )
#define SYSCALL_PREPARE( Syscall )      SyscallPrepare( Syscall )
#define SYSCALL_INVOKE( Syscall, ... )  SyscallInvoke( __VA_ARGS__ )
 
/*!
 * @brief
 *  Resolve syscall information (SSN, address...)
 *
 * @param Hash
 *  hash of the syscall name
 *
 * @return
 *  pointer to a data structure containing information about the syscall
 */
PSYSCALL SyscallResolve(
  _In_ ULONG Hash
);

#else
  #define SYSCALL_INIT( x )               __typeof__( x ) * x;
  #define SYSCALL_RESOLVE( Hash )         LdrFunctionAddr( Instance.Modules.Ntdll, Hash )
  #define SYSCALL_PREPARE( Syscall )
  #define SYSCALL_INVOKE( Syscall, ... )  Syscall( __VA_ARGS__ ) // Syscall is the function address
#endif

#endif //CORE_SYSCALL_H
