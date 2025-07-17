#include "imperium/macros.h"

#include <imperium/crypto.h>
#include <imperium/syscall.h>

namespace imperium::syscall {
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
  declfn NTSTATUS resolve( _In_ SYMBOL_HASH SyscallHash, _Out_ PSYSCALL Syscall ) {
    byte* SyscallAddr      = { 0 };
    byte* FirstSyscallAddr = { 0 };
    void* Ntdll            = { 0 };

    //
    // sanity check
    //
    if ( ! Syscall || ! SyscallHash.Function || ! SyscallHash.Module ) {
      return STATUS_INVALID_PARAMETER;
    }

    //
    // check ntdll address
    //
    if ( ! ( Ntdll = ldr::module( SyscallHash.Module ) ) ) {
      return STATUS_INTERNAL_ERROR;
    }

    //
    // get the address of the first syscall and the one we want to resolve
    //
    if ( ! ( SyscallAddr = static_cast< PBYTE >( ldr::function( Ntdll, SyscallHash.Function ) ) ) ||
         ! ( FirstSyscallAddr = static_cast< PBYTE >( ldr::function( Ntdll, H_STR( "NtAccessCheck" ) ) ) ) ) {
      return STATUS_INTERNAL_ERROR;
    }

    //
    // make sure we got the address of the first syscall
    // in case NtAccessCheck is no longer the first syscall
    // might break because of NtQuerySystemTime
    // look for the syscall & ret instruction
    // as long as we find some it means we aren't at the first syscall
    //
    while ( *( FirstSyscallAddr - 0x0E ) == 0x0F && *( FirstSyscallAddr - 0x0D ) == 0x05 &&
            *( FirstSyscallAddr - 0x0C ) == 0xC3 ) {
      FirstSyscallAddr -= 32;
    }

    //
    // calculate the SSN
    // and add the address
    //
    Syscall->Address = SyscallAddr;
    Syscall->Ssn     = ( SyscallAddr - FirstSyscallAddr ) / 32;

    //
    // handle the case of ntdll!NtQuerySystemTime
    // it use to be a system call but no longer is
    // so its syscall stub is just a jmp instruction and is not 32 bytes
    // kinda of mess all offset from there
    //
    if ( ( SyscallAddr - FirstSyscallAddr ) % 32 != 0 ) {
      Syscall->Ssn++;
    }

    return STATUS_SUCCESS;
  }
}  // namespace imperium::syscall
