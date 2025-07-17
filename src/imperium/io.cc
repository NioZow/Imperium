#include <cstdint>
#include <imperium/crypto.h>
#include <imperium/defs.h>
#include <imperium/instance.h>
#include <imperium/macros.h>
#include <imperium/mem.h>
#include <imperium/win32.h>

namespace imperium::io {
  /*!
   * took from havoc, credits go to 5pider
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
  declfn void printf( IN const char* fmt, ... ) {
    IMPERIUM_INSTANCE

    uint32_t OutputSize   = { 0 };
    char*    OutputString = { 0 };
    va_list  VaListArg    = { 0 };

    //
    // sanity check
    //
    if ( ! fmt ) {
      return;
    }

    //
    // get the handle to the output console
    //
    if ( ! Instance()->ConsoleOutput ) {
      win32::call< fnAttachConsole >( H_FUNC( "kernel32!AttachConsole" ), ATTACH_PARENT_PROCESS );

      if ( ! ( Instance()->ConsoleOutput =
                   win32::call< fnGetStdHandle >( H_FUNC( "kernel32!GetStdHandle" ), STD_OUTPUT_HANDLE ) ) ) {
        return;
      }
    }

    va_start( VaListArg, fmt );

    //
    // allocate space for the final string
    //
    OutputSize   = win32::call< fnVsnprintf >( H_FUNC( "msvcrt!vsnprintf" ), nullptr, 0, fmt, VaListArg ) + 1;
    OutputString = static_cast< PCHAR >( mem::alloc( OutputSize ) );

    //
    // write the final string
    //
    win32::call< fnVsnprintf >( H_FUNC( "msvcrt!vsnprintf" ), OutputString, OutputSize, fmt, VaListArg );

    //
    // write it to the console
    //
    win32::call< fnWriteConsoleA >( H_FUNC( "kernel32!WriteConsoleA" ), Instance()->ConsoleOutput, OutputString,
        OutputSize, nullptr, nullptr );

    //
    // free the string
    //
    mem::zero( OutputString, OutputSize );
    mem::free( OutputString );

    va_end( VaListArg );
  }
}  // namespace imperium::io
