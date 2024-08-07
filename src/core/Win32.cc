#include <Common.h>
#include <core/Win32.h>

/*!
 * take from havoc, credits go to 5pider
 *
 * @brief
 *  custom printf implementation
 *
 * @param fmt
 *  format of the string
 *
 * @param ...
 *  printf parameters
 */
FUNC VOID LogToConsole(
    IN PCSTR fmt,
    ...
) {
    STARDUST_INSTANCE;

    INT     OutputSize   = { 0 };
    PCHAR   OutputString = { 0 };
    va_list VaListArg    = { 0 };

    //
    // sanity check
    //
    if ( ! fmt ) {
        return;
    }

    //
    // get the handle to the output console
    //
    if ( ! Self->ConsoleOutput ) {
        Self->Win32.AttachConsole( ATTACH_PARENT_PROCESS );

        if ( ! ( Self->ConsoleOutput = Self->Win32.GetStdHandle( STD_OUTPUT_HANDLE ) ) ) {
            return;
        }
    }

    va_start( VaListArg, fmt );

    //
    // allocate space for the final string
    //
    OutputSize   = Self->Win32.vsnprintf( NULL, 0, fmt, VaListArg ) + 1;
    OutputString = Self->Win32.RtlAllocateHeap( NtProcessHeap(), HEAP_ZERO_MEMORY, OutputSize );

    //
    // write the final string
    //
    Self->Win32.vsnprintf( OutputString, OutputSize, fmt, VaListArg );

    //
    // write it to the console
    //
    Self->Win32.WriteConsoleA( Self->ConsoleOutput, OutputString, OutputSize, NULL, NULL );

    Imperium::mem::zero( OutputString, OutputSize );
    Self->Win32.RtlFreeHeap( NtProcessHeap(), 0, OutputString );

    va_end( VaListArg );
}
