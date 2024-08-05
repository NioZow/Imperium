#include <Common.h>
#include <core/Runtime.h>

FUNC VOID Main(
    IN PVOID Param
) {
    STARDUST_INSTANCE

    PVOID             Message       = { 0 };
    NTSTATUS          NtStatus      = { 0 };
    HANDLE            File          = { 0 };
    UNICODE_STRING    Path          = INIT_UNICODE_STRING( L"\\??\\C:\\flag.txt" );
    OBJECT_ATTRIBUTES ObjAttrs      = { 0 };
    IO_STATUS_BLOCK   IoStatusBlock = { 0 };

    Message = NtCurrentPeb()->ProcessParameters->ImagePathName.Buffer;

    SYSCALL NtCreateFile = { 0 };

    SYSCALL_RESOLVE( HASH_STR( "NtCreateFile" ), NtCreateFile );

    PRINTF_INFO( "Address of NtCreateFile: 0x%08X\n", NtCreateFile.Address );
    PRINTF_INFO( "SSN of NtCreateFile: 0x%X\n", NtCreateFile.Ssn );

    InitializeObjectAttributes( &ObjAttrs, &Path, OBJ_CASE_INSENSITIVE, NULL, NULL );

    SYSCALL_PREPARE( NtCreateFile );
    if ( ! NT_SUCCESS( NtStatus = SYSCALL_INVOKE(
        NtSyscall.NtCreateFile,
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

    //
    // pop da message
    //
    Self->Win32.MessageBoxW( NULL, Message, L"cant stop", MB_OK );
}
