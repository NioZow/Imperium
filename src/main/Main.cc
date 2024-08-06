#include <Common.h>
#include <core/Runtime.h>

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

    Self->Syscall = &SysNtCreateFile;

    Message = NtCurrentPeb()->ProcessParameters->ImagePathName.Buffer;

    SYSCALL_RESOLVE( HASH_STR( "NtCreateFile" ) );

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

    //
    // pop da message
    //
    Self->Win32.MessageBoxW( NULL, Message, L"What I miss", MB_OK );
}
