#ifndef IMPERIUM_SYSCALL
#define IMPERIUM_SYSCALL

#include <imperium/defs.h>
#include <imperium/macros.h>
#include <imperium/win32.h>
#include <utility>

/*!
 * @brief
 *  perform indirect syscall
 *
 * @param ...
 *  parameters of the syscall
 *
 * @return
 *  return status of the syscall
 */
extern "C" NTSTATUS SyscallIndirect( IN OUT OPTIONAL...  // args
);

/*!
 * @brief
 *  perform direct syscall
 *
 * @param ...
 *  parameters of the syscall
 *
 * @return
 *  return status of the syscall
 */
extern "C" NTSTATUS Direct( IN OUT OPTIONAL...  // args
);


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
  NTSTATUS resolve( _In_ SYMBOL_HASH SyscallHash, _In_ PSYSCALL Syscall );

  /*!
   * @brief
   *  call a nt function
   *  just forward the call to win32::call
   *
   * @tparam Func
   *  type of the function
   *
   * @tparam Args
   *  arguments to be passed to the function
   *  only the right number of arguments will be accepted
   *
   * @param FuncHash
   *  the hash of the dll and the hash of the func's name
   *
   * @param args
   *  args to pass to the function
   *
   * @return
   *  return value of syscall
   */
  template< typename Func, class... Args >
  inline NTSTATUS call( SYMBOL_HASH FuncHash, Args... args ) {
    return win32::call< Func >( FuncHash, args... );
  }

  /*!
   * @brief
   *  perform indirect syscall
   *
   * @tparam Func
   *  type of the function
   *
   * @tparam Args
   *  arguments to be passed to the function
   *  only the right number of arguments will be accepted
   *
   * @param SymHash
   *  the hash of the dll and the hash of the func's name
   *
   * @param args
   *  args to pass to the function
   *
   * @return
   *  return value of syscall
   */
  template< typename Func, class... Args >
  inline NTSTATUS indirect( SYMBOL_HASH SymHash, Args... args ) {
    //
    // resolve the syscall
    //
    if ( ! win32::resolve( SymHash, SymbolSyscall | SyscallAddInstance ) ) {
      PRINTF_ERROR( "Failed to resolve symbol with module 0x%08X and function 0x%08X", SymHash.Module,
          SymHash.Function );
      return STATUS_INTERNAL_ERROR;
    }

    //
    // perform indirect syscall
    //
    return SyscallIndirect( std::forward< Args >( args )... );
  }

  /*!
   * @brief
   *  perform direct syscall
   *
   * @tparam Func
   *  type of the function
   *
   * @tparam Args
   *  arguments to be passed to the function
   *  only the right number of arguments will be accepted
   *
   * @param SymHash
   *  the hash of the dll and the hash of the func's name
   *
   * @param args
   *  args to pass to the function
   *
   * @return
   *  return value of syscall
   */
  template< typename Func, class... Args >
  inline NTSTATUS direct( SYMBOL_HASH SymHash, Args... args ) {
    //
    // resolve the syscall
    //
    if ( ! win32::resolve( SymHash, SymbolSyscall | SyscallAddInstance ) ) {
      PRINTF_ERROR( "Failed to resolve symbol with module 0x%08X and function 0x%08X", SymHash.Module,
          SymHash.Function );
      return STATUS_INTERNAL_ERROR;
    }

    //
    // perform direct syscall
    //
    return SyscallDirect( std::forward< Args >( args )... );
  }
}  // namespace imperium::syscall

#endif  // IMPERIUM_SYSCALL
