#include <cstdint>
#include <imperium/crypto.h>
#include <imperium/syscall.h>

namespace imperium {
  /*!
   *  WARNING: This resolution technique will only work as long as those conditions are met :
   *  - NtAccessCheck is the first system call
   *  - Syscall stubs are 32 bytes
   *  - Syscall are all stored in a row, sorted by SSN number
   *  - Only works for syscalls located in ntdll, not those in win32u
   *  - There is only one "fake" syscall : NtQuerySystemTime (that special case is handled)
   *
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
  declfn syscall_t syscall_t::resolve( _In_ symbol_t symbol ) {
    instance_t* instance         = instance_t::find();
    uint8_t*    syscall_addr     = { 0 };
    byte*       FirstSyscallAddr = { 0 };
    syscall_t   syscall          = { 0 };

    //
    // sanity check
    //
    if ( ! symbol.function || ! symbol.module ) {
      return syscall;
    }

    //
    // get the first syscall address
    // if not already resolved
    //
    if ( ! instance->first_syscall ) {
      //
      //
      //
      if ( ! ( instance->first_syscall =
                   static_cast< uint8_t* >( win32_t::resolve( H_FUNC( "ntdll!NtAccessCheck" ) ).function_address ) ) )
        return syscall;
    }

    //
    // get the syscall address
    //
    if ( ! ( syscall_addr = static_cast< uint8_t* >( win32_t::resolve( symbol ).function_address ) ) ) return syscall;

    //
    // calculate the SSN
    //
    syscall.address = syscall_addr;
    syscall.ssn     = ( syscall_addr - instance->first_syscall ) / 32;

    //
    // handle the case of ntdll!NtQuerySystemTime
    // it use to be a system call but no longer is
    // so its syscall stub is just a jmp instruction and is not 32 bytes
    // kinda of mess all offset from there
    //
    if ( ( syscall_addr - FirstSyscallAddr ) % 32 != 0 ) syscall.ssn++;
    return syscall;
  }
}  // namespace imperium
