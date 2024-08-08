#include <common/Imperium.h>

FUNC VOID Main(
    IN PVOID Param
) {
    STARDUST_INSTANCE

    PVOID             Message         = { 0 };
    NTSTATUS          NtStatus        = { 0 };
    HANDLE            File            = { 0 };
    UNICODE_STRING    Path            = INIT_UNICODE_STRING( L"\\??\\C:\\flag.txt" );
    OBJECT_ATTRIBUTES ObjAttrs        = { 0 };
    IO_STATUS_BLOCK   IoStatusBlock   = { 0 };
    SYSCALL           SysNtCreateFile = { 0 };
    BYTE              Buffer[ 4 ]     = { 1, 2, 3, 4 };
    CHAR              String[ ]       = "noah";

    Imperium::util::string::upper( String );

    PRINTF( "String: %s %d\n", String, Imperium::util::string::len( "NOAH" ) );

    /*
    PVOID Module = Imperium::ldr::module( H_STR( "kernel32.dll" ) );

    PVOID Address = Imperium::win32::call< fnGetModuleHandleA >(
        H_FUNC( "kernel32!GetModuleHandleA" ),
        "kernel32.dll"
    );

    if ( ! NT_SUCCESS( NtStatus = Imperium::syscall::resolve(H_FUNC("ntdll!NtCreateFile"), & SysNtCreateFile) ) ) {
        PRINTF_ERROR( "Failed to resolve SysNtCreateFile with error: 0x%08X\n", NtStatus );
        return;
    }

    InitializeObjectAttributes( &ObjAttrs, &Path, OBJ_CASE_INSENSITIVE, NULL, NULL );

    if ( ! NT_SUCCESS( NtStatus = Imperium::syscall::call<fnNtCreateFile>(
        H_FUNC("ntdll!NtCreateFile"),
        &File, FILE_GENERIC_READ, &ObjAttrs, &IoStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0
    ) ) ) {
        PRINT_NT_ERROR( "NtCreateFile", NtStatus );
        return;
    }

    PRINTF( "NtCreateFile SSN: 0x%X\n", SysNtCreateFile.Ssn );
    PRINTF( "NtCreateFile Address: 0x%X\n", SysNtCreateFile.Address );
    PRINTF( "Kernel32 is at 0x%08X 0x%08X\n", Address, Module );
    */

    //
    // pop da message
    //
    PRINTF_INFO( "[BUILD::%s] Execution succeeded!", __TIME__ );
}
