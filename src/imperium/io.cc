#include <cstdint>
#include <imperium/crypto.h>
#include <imperium/defs.h>
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
  declfn void printf( _In_ const char* fmt, ... ) {
    instance_t*   instance = instance_t::find();
    buf_t< char > output   = { 0 };
    va_list       args     = { 0 };

    //
    // sanity check
    //
    if ( ! fmt ) return;

    //
    // get the handle to the output console
    //
    if ( ! instance->console ) {
      win32::call< fnAttachConsole >( H_FUNC( "kernel32!AttachConsole" ), ATTACH_PARENT_PROCESS );

      if ( ! ( instance->console =
                   win32::call< fnGetStdHandle >( H_FUNC( "kernel32!GetStdHandle" ), STD_OUTPUT_HANDLE ) ) )
        return;
    }

    va_start( args, fmt );

    //
    // allocate space for the final string
    //
    output.len  = win32::call< fnVsnprintf >( H_FUNC( "msvcrt!vsnprintf" ), nullptr, 0, fmt, args ) + 1;
    output.data = static_cast< char* >( mem::alloc( output.len ) );

    //
    // write the final string
    //
    win32::call< fnVsnprintf >( H_FUNC( "msvcrt!vsnprintf" ), output.data, output.len, fmt, args );

    //
    // write it to the console
    //
    win32::call< fnWriteConsoleA >( H_FUNC( "kernel32!WriteConsoleA" ), instance->console, output.data, output.len,
        nullptr, nullptr );

    //
    // free the string
    //
    mem::zfree( &output );

    va_end( args );
  }

  declfn void print_bytes( uint8_t* data, size_t size ) {
    io::printf( "[" );
    for ( size_t i = 0; i < size; ++i ) {
      if ( i != size - 1 )
        io::printf( " 0x%02x,", data[ i ] );
      else
        io::printf( " 0x%02x", data[ i ] );
    }
    io::printf( " ]\n" );
  }

  declfn void print_hex( uint8_t* data, size_t size ) {
    for ( size_t i = 0; i < size; ++i ) {
      io::printf( "%02x", data[ i ] );
    }
    io::printf( "\n" );
  }
}  // namespace imperium::io
