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
        PSYMBOL add(
            SYMBOL_HASH SymHash,
            PVOID       SymAddr,
            USHORT      Ssn
        );

        PSYMBOL get(
            PSYMBOL_HASH SymbolHash
        );
    }
}

