#ifndef IMPERIUM_INSTANCE
#define IMPERIUM_INSTANCE

#include <imperium/defs.h>

namespace imperium::instance {
  /*!
   * @brief
   *  get a pointer to the instance by reading the peb
   *  this one in located in the process heaps table
   *
   * @return
   *  pointer to the instance
   */
  PINSTANCE get();
  PINSTANCE init();
  void      start();

  namespace symbol {
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
    PSYMBOL add( _In_ SYMBOL_HASH SymHash, _In_ void* SymAddr, _In_ uint16_t Ssn );

    PSYMBOL get( _In_ PSYMBOL_HASH SymbolHash );
  }  // namespace symbol
}  // namespace imperium::instance

#endif  // IMPERIUM_INSTANCE
