#include <imperium/defs.h>
#include <imperium/instance.h>
#include <imperium/macros.h>
#include <imperium/syscall.h>

namespace imperium::win32 {
  /*!
   * @brief
   *  get the address of a function
   *
   * @param SymHash
   *  hashed symbol of the func
   *
   * @param Flags
   *  flags
   *
   * @return
   *  symbol
   */
  FUNC PSYMBOL resolve( SYMBOL_HASH SymHash, ULONG Flags ) {
    IMPERIUM_INSTANCE

    NTSTATUS NtStatus = { 0 };
    PSYMBOL  Sym      = { 0 };
    PVOID    Module   = { 0 };
    SYSCALL  Func     = { 0 };

    //
    // check if the function had already been resolved before
    //
    if ( ! ( Sym = instance::symbol::get( &SymHash ) ) || ( ! Sym->Syscall.Ssn && Flags & SymbolSyscall ) ) {
      //
      // the function has not been resolved before
      //
      if ( Flags & SymbolSyscall ) {
        //
        // resolve the syscall
        //
        if ( ! NT_SUCCESS( NtStatus = syscall::resolve( SymHash, &Func ) ) ) {
          return nullptr;
        }
      } else {
        //
        // resolve the library from the peb
        //
        if ( ! ( Module = ldr::module( SymHash.Module ) ) ) {
          //
          // if it fails call LoadLibraryA (can't for now string hashed)
          // TODO: add support for that instead of loading the library to have it in peb
          //
          return nullptr;
        }

        //
        // resolve the func
        //
        if ( ! ( Func.Address = ldr::function( Module, SymHash.Function ) ) ) {
          //
          // failed to resolve the func
          //
          return nullptr;
        }
      }

      //
      // store the function for later use
      //
      if ( ! ( Sym = instance::symbol::add( SymHash, Func.Address, Func.Ssn ) ) ) {
        return nullptr;
      }
    }

    //
    // add the syscall to the instance
    //
    if ( Flags & SymbolSyscall && Flags & SyscallAddInstance ) {
      Instance()->Syscall = &Sym->Syscall;
    }

    return Sym;
  }
} // namespace imperium::win32
