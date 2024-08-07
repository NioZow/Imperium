#ifndef CORE_SYSCALL_H
#define CORE_SYSCALL_H

#include <windows.h>

#ifdef INDIRECT_SYSCALL

typedef struct _SYSCALL {
    PVOID  Address;
    USHORT Ssn;
} SYSCALL, *PSYSCALL;

/*!
 * @brief
 *  perform indirect syscall
 *  the SyscallPrepare function must be called before calling the SyscallInvoke function
 *
 * @param ...
 *  parameters of the syscall
 *
 * @return
 *  return status of the syscall
 */
EXTERN_C NTSTATUS SyscallInvoke(
    IN OUT OPTIONAL ... // Args
);

/*!
 * @brief
 *  resolve syscall information (SSN, address...)
 *
 * @param SyscallHash
 *  hash of the syscall
 *
 * @param Syscall
 *  struct that will receive the address and ssn of the syscall
 *
 * @return
 *  pointer to a data structure containing information about the syscall
 */
NTSTATUS SyscallResolve(
    IN ULONG     SyscallHash,
    OUT PSYSCALL Syscall
);

#define SYSCALL_INIT( x )                       SYSCALL x
#define SYSCALL_RESOLVE( SyscallHash, Syscall ) SyscallResolve( SyscallHash, Syscall  )
#define SYSCALL_PREPARE( Syscall )              Self->Syscall = & Syscall
#define SYSCALL_PREPARE_STARDUST( Stardust, Syscall ) Stardust = & Syscall
#define SYSCALL_INVOKE( Syscall, ... )          SyscallInvoke( __VA_ARGS__ )
#define SYSCALL_RESOLVE_SUCCESS( Syscall )      NT_SUCCESS( Syscall )

#else
#define SYSCALL_INIT( x )                       __typeof__( x ) * x
#define SYSCALL_RESOLVE( SyscallHash )          LdrFunctionAddr( LdrModulePeb( HASH_STR( "ntdll.dll" ) ), SyscallHash )
#define SYSCALL_PREPARE( Syscall )
#define SYSCALL_INVOKE( Syscall, ... )          Syscall( __VA_ARGS__ ) // Syscall is the function address
#define SYSCALL_RESOLVE_SUCCESS( Syscall )      ( Syscall )
#endif

#endif //CORE_SYSCALL_H
