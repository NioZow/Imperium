#include <Common.h>
#include <core/Runtime.h>

FUNC RESOLVE_API_ERROR ResolveWinApi(
    OUT PINSTANCE Stardust
) {
    //
    // resolve kernel32 functions
    //
    if ( ! ( Stardust->Modules.Kernel32    = LdrModulePeb( H_STR( L"kernel32.dll" ) ) ) ||
         ! ( Stardust->Win32.AttachConsole = LdrFunctionAddr( Stardust->Modules.Kernel32, H_STR( "AttachConsole" ) ) ) ||
         ! ( Stardust->Win32.GetStdHandle  = LdrFunctionAddr( Stardust->Modules.Kernel32, H_STR( "GetStdHandle" ) ) ) ||
         ! ( Stardust->Win32.WriteConsoleA = LdrFunctionAddr( Stardust->Modules.Kernel32, H_STR( "WriteConsoleA" ) ) ) ||
         ! ( Stardust->Win32.LoadLibraryW  = LdrFunctionAddr( Stardust->Modules.Kernel32, H_STR( "LoadLibraryW" ) ) )
    ) {
        return RESOLVE_API_ERROR_MODULE_KERNEL32;
    }

    //
    // resolve msvcrt functions
    //
    if ( ! ( Stardust->Modules.Msvcrt  = LdrModulePeb( H_STR( L"msvcrt.dll" ) ) ) ||
         ! ( Stardust->Win32.vsnprintf = LdrFunctionAddr( Stardust->Modules.Msvcrt, H_STR( "vsnprintf" ) ) )
    ) {
        return RESOLVE_API_ERROR_MODULE_USER32;
    }

    //
    // resolve ntdll functions
    //
    if ( ! ( Stardust->Modules.Ntdll                = LdrModulePeb( H_STR( L"ntdll.dll" ) ) ) ||
         ! ( Stardust->Win32.RtlAllocateHeap        = LdrFunctionAddr( Stardust->Modules.Ntdll, H_STR( "RtlAllocateHeap" ) ) ) ||
         ! ( Stardust->Win32.RtlCreateHeap          = LdrFunctionAddr( Stardust->Modules.Ntdll, H_STR( "RtlCreateHeap" ) ) ) ||
         ! ( Stardust->Win32.RtlFreeHeap            = LdrFunctionAddr( Stardust->Modules.Ntdll, H_STR( "RtlFreeHeap" ) ) ) ||
         ! ( Stardust->Win32.RtlReAllocateHeap      = LdrFunctionAddr( Stardust->Modules.Ntdll, H_STR( "RtlReAllocateHeap" ) ) ) ||
         ! ( Stardust->Win32.NtProtectVirtualMemory = LdrFunctionAddr( Stardust->Modules.Ntdll, H_STR( "NtProtectVirtualMemory" ) ) )
        //! SYSCALL_RESOLVE_SUCCESS( SYSCALL_RESOLVE( HASH_STR( "NtProtectVirtualMemory" ), & Stardust->Win32.NtProtectVirtualMemory ) )
    ) {
        return RESOLVE_API_ERROR_MODULE_NTDLL;
    }

    //
    // resolve user32 functions
    //
    if ( ! ( Stardust->Modules.User32    = Stardust->Win32.LoadLibraryW( L"user32.dll" ) ) ||
         ! ( Stardust->Win32.MessageBoxW = LdrFunctionAddr( Stardust->Modules.User32, H_STR( "MessageBoxW" ) ) )
    ) {
        return RESOLVE_API_ERROR_MODULE_USER32;
    }

    return RESOLVE_API_SUCCESS;
}
