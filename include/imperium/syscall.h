#ifndef IMPERIUM_SYSCALL_H
#define IMPERIUM_SYSCALL_H

#include <imperium/defs.h>
#include <imperium/macros.h>
#include <imperium/win32.h>
#include <utility>

namespace imperium {
  struct syscall_t;

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
  extern "C" void SyscallConfig( syscall_t* syscall );

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
  extern "C" NTSTATUS SyscallInvoke( _Inout_opt_...  // args
  );

  struct syscall_t {
    void* address;

    //
    // a ssn can only be 32 bits and is usually is 16 bits
    // however setting to 64 bits to avoid problems because this is
    // directly written into `rax` in assembly.
    // This would probably have been padded to 16 bytes anyway but
    // I prefer handling this myself.
    //
    uint64_t ssn;

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
     *  data structure containing information about the syscall
     *  PERF: this structure is more than 8 bytes so it can't fit
     *  in a single register.
     *  Have to look how it is stored and if the perfs are good.
     */
    static syscall_t resolve( _In_ symbol_t symbol );

    /*!
     * @brief
     *  perform indirect syscall
     *
     * @tparam Func
     *  type of the syscall
     *
     * @tparam Args
     *  args to pass to the syscall
     *
     * @param args
     *  args to pass to the syscall
     *
     * @return
     *  syscall return value
     */
    template< typename Func, class... Args >
    inline auto exec( Args... args ) {
      if ( ! address || ! ssn ) __debugbreak();

      //
      // perform indirect syscall
      //
      SyscallConfig( this );
      return reinterpret_cast< Func >( SyscallInvoke )( std::forward< Args >( args )... );
    }
  };

  namespace syscall {
    /*
     * @brief
     *  resolve a syscall and execute it indirectly
     *
     * @tparam Func
     *  type of the syscall
     *
     * @tparam Args
     *  args to pass to the syscall
     *
     * @return
     *  syscall return value
     */
    template< typename Func, class... Args >
    inline auto indirect( symbol_t symbol, Args... args ) {
      return syscall_t::resolve( symbol ).exec< Func >( std::forward< Args >( args )... );
    }
  }  // namespace syscall
}  // namespace imperium


#endif  // IMPERIUM_SYSCALL_H
