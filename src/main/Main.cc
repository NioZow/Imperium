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

    PVOID Module = Imperium::ldr::module( H_STR( "kernel32.dll" ) );

    PVOID Address = Imperium::win32::call< fnGetModuleHandleA >(
        H_FUNC( "kernel32!GetModuleHandleA" ),
        "kernel32.dll"
    );

    PRINTF( "Kernel32 is at 0x%08X 0x%08X\n", Address, Module );

    /*
    Self->Syscall = &SysNtCreateFile;

    Message = NtCurrentPeb()->ProcessParameters->ImagePathName.Buffer;

    if ( ! NT_SUCCESS( NtStatus = SYSCALL_RESOLVE( HASH_STR( "NtCreateFile" ), Self->Syscall ) ) ) {
        PRINTF_ERROR( "SyscallResolve failed with error: 0x%08X\n", NtStatus );
    }

    PRINTF_INFO( "Hash of NtCreateFile 0x%08X\n", HASH_STR( "NtCreateFile" ) );
    PRINTF_INFO( "Address of NtCreateFile: 0x%08X\n", Self->Syscall->Address );
    PRINTF_INFO( "SSN of NtCreateFile: 0x%X\n", Self->Syscall->Ssn );

    InitializeObjectAttributes( &ObjAttrs, &Path, OBJ_CASE_INSENSITIVE, NULL, NULL );

    if ( ! NT_SUCCESS( NtStatus = SYSCALL_INVOKE(
        Self->Syscall,
        &File,
        FILE_GENERIC_READ,
        &ObjAttrs,
        &IoStatusBlock,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ,
        FILE_OPEN,
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        0
    ) ) ) {
        PRINT_NT_ERROR( "NtCreateFile", NtStatus );
    }

    PRINTF( "NtStatus: 0x%08X\nFile: 0x%03X", NtStatus, File );
    */

    //
    // pop da message
    //
    //Self->Win32.MessageBoxW( NULL, Message, L"What I miss", MB_OK );
    PRINTF_INFO( "[BUILD::%s] Execution succeeded!", __TIME__ );
}
