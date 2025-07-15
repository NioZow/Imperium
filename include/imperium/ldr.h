#ifndef IMPERIUM_LDR
#define IMPERIUM_LDR

#include <windows.h>

namespace imperium::ldr {
    /*!
        * 5pider implementation, credits go to him
        *
        * @brief
        *  get the address of a module
        *
        * @param ModuleHash
        *  hash of the module to get
        *
        * @return
        *  address of the DLL base ( NULL if not found )
        */
    PVOID module(
        IN ULONG Hash
    );

    /*!
        * 5pider implementation, credits go to him
        *
        * @brief
        *  load the address of a function from base DLL address
        *
        * @param Module
        *  base address of the DLL
        *
        * @param FunctionHash
        *  hash of the function to get the address of
        *
        * @return
        *  address of the function ( NULL if not found )
        */
    PVOID function(
        IN PVOID Library,
        IN ULONG Function
    );
}

#endif // IMPERIUM_LDR
