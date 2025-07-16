#include <imperium/crypto.hpp>
#include <imperium/defs.h>
#include <imperium/ldr.h>
#include <imperium/macros.h>

namespace imperium::instance {
  /*!
   * @brief
   *  get a pointer to the instance by reading the peb
   *  this one in located in the process heaps table
   *
   * @return
   *  pointer to the instance
   */
  FUNC PINSTANCE get() {
    PINSTANCE Instance = { 0 };
    ULONG     Context  = { 0 };

    for ( int i = 0; i < NtCurrentPeb()->NumberOfHeaps; i++ ) {
      Context = ( ( PINSTANCE ) ( NtCurrentPeb()->ProcessHeaps[ i ] ) )->Context;

      if ( Context == 0xc0debabe ) {
        Instance = static_cast< PINSTANCE >( NtCurrentPeb()->ProcessHeaps[ i ] );
        break;
      }
    }

    return Instance;
  }

  namespace symbol {
    /*!
     * @brief
     *  get a pointer to a function info if it is already stored in memory
     *
     * @return
     *  function address struct
     */
    FUNC PSYMBOL get( PSYMBOL_HASH FuncHash ) {
      IMPERIUM_INSTANCE

      PSYMBOL FuncAddr = Instance()->Symbol;

      //
      // if there is no function loaded quit now
      //
      if ( ! Instance()->Symbol ) {
        return NULL;
      }

      //
      // iterate through all loaded functions
      //
      do {
        //
        // search if a function has the same hashes and so is the same
        //
        if ( FuncAddr->FunctionHash == FuncHash->Function && FuncAddr->ModuleHash == FuncHash->Module ) {
          break;
        }
      } while ( ( FuncAddr = FuncAddr->Next ) );

      return FuncAddr;
    }

    /*!
     * @brief
     *  store the function to load only once
     *
     * @param SymHash
     *  hashes of the func
     *
     * @param SymAddr
     *  addr of the module/function
     *
     * @param Ssn
     *  ssn of the syscall
     */
    FUNC PSYMBOL add( SYMBOL_HASH SymHash, PVOID SymAddr, USHORT Ssn = 0 ) {
      IMPERIUM_INSTANCE

      PSYMBOL          *Sym    = &Instance()->Symbol;
      PVOID             Module = { 0 };
      fnRtlAllocateHeap Func   = { 0 };

      //
      // get the address of the last symbol
      //
      while ( *Sym && ( Sym = &( *Sym )->Next ) );

      //
      // allocate the mem manually
      // cant call mem:alloc cuz it calls win32::call
      // creates a recursive infinite loop
      // todo: find a better way to do this?
      // does not fix anything still does not work
      //
      if ( ! ( Module = ldr::module( H_STR( "ntdll.dll" ) ) ) ) {
        return nullptr;
      }

      if ( ! ( Func = reinterpret_cast< fnRtlAllocateHeap >( ldr::function( Module, H_STR( "RtlAllocateHeap" ) ) ) ) ) {
        return nullptr;
      }

      *Sym = static_cast< PSYMBOL >( Func( NtCurrentHeap(), HEAP_ZERO_MEMORY, sizeof( SYMBOL ) ) );

      //
      // set the symbol
      //
      ( *Sym )->FunctionHash = SymHash.Function;
      ( *Sym )->ModuleHash   = SymHash.Module;
      ( *Sym )->Address      = SymAddr;
      ( *Sym )->Syscall.Ssn  = Ssn;

      return *Sym;
    }
  } // namespace symbol
} // namespace imperium::instance
