#include <common/Imperium.h>

FUNC VOID Main(
    IN PVOID Param
) {
    STARDUST_INSTANCE

    PRINTF_INFO( "[BUILD::%s] Execution started\n\n", __TIME__ );

    NTSTATUS          NtStatus      = { 0 };
    HANDLE            File          = { 0 };
    UNICODE_STRING    Path          = INIT_UNICODE_STRING( L"\\??\\C:\\flag.txt" );
    OBJECT_ATTRIBUTES ObjAttrs      = { 0 };
    IO_STATUS_BLOCK   IoStatusBlock = { 0 };

    InitializeObjectAttributes( &ObjAttrs, &Path, OBJ_CASE_INSENSITIVE, NULL, NULL );

    NtStatus = Imperium::syscall::direct< fnNtCreateFile >(
        H_FUNC( "ntdll!NtCreateFile" ),
        &File, FILE_GENERIC_READ, &ObjAttrs, &IoStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0
    );

    PRINTF( "=> NtStatus: 0x%08X\n", NtStatus );

    //
    // pop da message
    //
    PRINTF_INFO( "[BUILD::%s] Execution succeeded!", __TIME__ );
}
