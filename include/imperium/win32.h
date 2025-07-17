#ifndef IMPERIUM_WIN32
#define IMPERIUM_WIN32

#include <imperium/defs.h>
#include <imperium/ldr.h>
#include <utility>

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
  PSYMBOL resolve( SYMBOL_HASH SymHash, ULONG Flags );

  /*!
   * @brief
   *  call a win32/nt function
   *
   * @tparam Func
   *  type of the function, e.g.
   *  typedef PVOID ( WINAPI*fnGetModuleHandleA )(
   *      PSTR Module
   *  );
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
   *  return value of win32 call
   */
  template< typename Func, class... Args >
  inline auto call( SYMBOL_HASH SymHash, Args... args ) {
    //
    // resolving using win32::resolve does not work there
    // can't figure out, i suspected this was a recursive issue
    // with calling mem::alloc but in my test it still crash
    //
    void* Module   = ldr::module( SymHash.Module );
    Func  Function = reinterpret_cast< Func >( ldr::function( Module, SymHash.Function ) );

    return Function( std::forward< Args >( args )... );
    /*
    PSYMBOL Sym = { 0 };

    //
    // resolve the func
    //
    if ( ! ( Sym = resolve( SymHash, 0 ) ) ) {
        //
        // failed to resolve the function
        // no proper error handling for now
        //
        //PRINTF_ERROR( "Failed to resolve symbol with module 0x%08X and function 0x%08X", SymHash.Module,
    SymHash.Function );
        __debugbreak();
    }

    return ( ( Func ) Sym->Address )( std::forward< Args >( args )... );
    */
  }
}  // namespace imperium::win32

#endif  // IMPERIUM_WIN32
