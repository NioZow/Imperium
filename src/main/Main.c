#include <Common.h>

FUNC VOID Main(
    _In_ PVOID Param
) {
    STARDUST_INSTANCE

    PVOID Message = { 0 };

    //
    // resolve kernel32.dll related functions
    //
    if ( ( Instance()->Modules.Kernel32 = LdrModulePeb( HASH_STRW( L"KERNEL32.DLL", 24 ) ) ) ) {
        if ( ! ( Instance()->Win32.LoadLibraryW = LdrFunctionAddr( Instance()->Modules.Kernel32, HASH_STR( "LoadLibraryW" ) ) ) ) {
            return;
        }
    }

    //
    // resolve user32.dll related functions
    //
    if ( ( Instance()->Modules.User32 = Instance()->Win32.LoadLibraryW( L"USER32" ) ) ) {
        if ( ! ( Instance()->Win32.MessageBoxW = LdrFunctionAddr( Instance()->Modules.User32, HASH_STR( "MessageBoxW" ) ) ) ) {
            return;
        }
    }

    Message = NtCurrentPeb()->ProcessParameters->ImagePathName.Buffer;

    //
    // pop da message
    //
    Instance()->Win32.MessageBoxW( NULL, Message, L"Stardust MessageBox", MB_OK );
}

