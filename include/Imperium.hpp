#ifndef IMPERIUM_H
#define IMPERIUM_H

// imperium individual headers
#include <imperium/crypto.hpp>
#include <imperium/defs.h>
#include <imperium/instance.h>
#include <imperium/io.h>
#include <imperium/ldr.h>
#include <imperium/macros.h>
#include <imperium/memory.h>
#include <imperium/native.h>
#include <imperium/syscall.h>
#include <imperium/util.h>
#include <imperium/win32.h>

#ifdef IMPERIUM_SHELLCODE

VOID Main(
    IN PVOID Params
);

/*!
 * @brief
 *  function to start the program from assembly
 *  the entry for your payload should rather be the Main function
 *  as this one is called by that func
 *
 * @param Param
 *  parameters
 */
EXTERN_C FUNC VOID PreMain(
    PVOID Params
) {
    PINSTANCE Instance = { 0 };
    PPVOID    MmAddr   = { 0 };
    PPEB      Peb      = NtCurrentPeb();

    //
    // check if there are enough heaps to hold our instance
    //
    if ( Peb->NumberOfHeaps >= Peb->MaximumNumberOfHeaps ) {
        return;
    }

    //
    // get the address of last heap to use to store a pointer to our instance
    //
    MmAddr = &Peb->ProcessHeaps[ Peb->NumberOfHeaps++ ];

    //
    // allocate memory for the instance
    //
    if ( ! ( *MmAddr = imperium::mem::alloc( sizeof( INSTANCE ) ) ) ) {
        return;
    }

    //
    // set a context to find the instance struct in memory
    //
    Instance = static_cast<PINSTANCE>( *MmAddr );
    Instance->Context = 0xc0debabe;

    //
    // get the base address of the current implant in memory and the end.
    // subtract the implant end address with the start address you will
    // get the size of the implant in memory
    //
    Instance->Base.Buffer = StRipStart();
    Instance->Base.Length = U_PTR( StRipEnd() ) - U_PTR( Instance->Base.Buffer );

    //
    // now execute the implant entrypoint
    //
    Main( Params );
}
#elif IMPERIUM_EXE
INT Main(
    IN INT argc,
    IN PCSTR argv[]
);

INT main(
    IN INT argc,
    IN PCSTR argv[]
) {
    PINSTANCE Instance = { 0 };
    PPVOID    MmAddr   = { 0 };
    PPEB      Peb      = NtCurrentPeb();

    //
    // check if there are enough heaps to hold our instance
    //
    if ( Peb->NumberOfHeaps >= Peb->MaximumNumberOfHeaps ) {
        return EXIT_FAILURE;
    }

    //
    // get the address of last heap to use to store a pointer to our instance
    //
    MmAddr = &Peb->ProcessHeaps[ Peb->NumberOfHeaps++ ];

    //
    // allocate memory for the instance
    //
    if ( ! ( *MmAddr = imperium::mem::alloc( sizeof( INSTANCE ) ) ) ) {
        return EXIT_FAILURE;
    }

    //
    // set a context to find the instance struct in memory
    //
    Instance = static_cast<PINSTANCE>( *MmAddr );
    Instance->Context = 0xc0debabe;

    //
    // now execute the implant entrypoint
    //
    return Main( argc, argv );
}

#elif IMPERIUM_BOF
INT Main(
    IN INT argc,
    IN PCSTR argv[]
);

VOID go(
    IN INT argc,
    IN PCSTR argv[]
) {
    PINSTANCE Instance = { 0 };
    PPVOID    MmAddr   = { 0 };
    PPEB      Peb      = NtCurrentPeb();

    //
    // check if there are enough heaps to hold our instance
    //
    if ( Peb->NumberOfHeaps >= Peb->MaximumNumberOfHeaps ) {
        return;
    }

    //
    // get the address of last heap to use to store a pointer to our instance
    //
    MmAddr = &Peb->ProcessHeaps[ Peb->NumberOfHeaps++ ];

    //
    // allocate memory for the instance
    //
    if ( ! ( *MmAddr = Instance = imperium::mem::alloc( sizeof( INSTANCE ) ) ) ) {
        return;
    }

    //
    // set a context to find the instance struct in memory
    //
    Instance->Context = 0xc0debabe;

    //
    // now execute the implant entrypoint
    //
    Main( argc, argv );
}
#endif //IMPERIUM_*

#endif //IMPERIUM_H
