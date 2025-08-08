#ifndef IMPERIUM_MACROS_H
#define IMPERIUM_MACROS_H

#include <imperium/crypto.h>
#include <imperium/defs.h>
#include <imperium/io.h>
#include <imperium/win32.h>

#ifdef IMPERIUM_PIC
  #define declfn __attribute__( ( section( ".text$B" ) ) )
extern "C" void* StRipStart();
extern "C" void* StRipEnd();
#else
  #define declfn
  #define StRipStart() nullptr
  #define StRipEnd()   nullptr
#endif

//
// pseudo handles
//
#define NtCurrentProcessToken()         reinterpret_cast< void* >( static_cast< uint64_t >( -4 ) )
#define NtCurrentThreadToken()          reinterpret_cast< void* >( static_cast< uint64_t >( -5 ) )
#define NtCurrentThreadEffectiveToken() reinterpret_cast< void* >( static_cast< uint64_t >( -6 ) )

//
// peb/teb related macros
//
#define NtCurrentHeap()    static_cast< void* >( NtCurrentPeb()->ProcessHeap )
#define NtProcessHeap()    NtCurrentHeap()
#define ZwCurrentProcess() NtCurrentProcess()
#define ZwCurrentThread()  NtCurrentThread()
#define NtProcessImage()   static_cast< wchar_t* >( NtCurrentPeb()->ProcessParameters->ImagePathName.Buffer )
#define NtProcessCurrentDirectory() \
  static_cast< wchar_t* >( NtCurrentPeb()->ProcessParameters->CurrentDirectory.DosPath )

#if _WIN64
  #define NtCurrentProcessId() reinterpret_cast< uint32_t >( __readgsdword( 0x40 ) )
#elif _WIN32
  #define NtCurrentProcessId() reinterpret_cast< uint32_t >( __readgsdword( 0x20 ) )
#endif

#if _WIN64
  #define NtCurrentThreadId() reinterpret_cast< uint32_t >( __readgsdword( 0x48 ) )
#elif _WIN32
  #define NtCurrentThreadId() reinterpret_cast< uint32_t >( __readgsdword( 0x24 ) )
#endif

//
// io macros
//
#define PRINTF( text, ... )       imperium::io::printf( text, ##__VA_ARGS__ );
#define PRINTF_INFO( text, ... )  PRINTF( "[*] " text "\n", ##__VA_ARGS__ );
#define PRINTF_ERROR( text, ... ) PRINTF( "[!] " text "\n", ##__VA_ARGS__ );
#ifdef IMPERIUM_RELEASE
  #define PRINTF_DBG( text, ... )

  #define PRINT_WIN32_ERROR( winapi )                                    \
    {                                                                    \
      ENC_STRING( error, winapi );                                       \
      PRINTF_ERROR( "%s failed with error: %ld", error, NtLastError() ); \
    }

  #define PRINT_NT_ERROR( ntapi, status )                         \
    {                                                             \
      ENC_STRING( error, ntapi );                                 \
      PRINTF_ERROR( "%s failed with error: %ld", error, status ); \
    }
#else
  #define PRINTF_DBG( text, ... ) \
    imperium::io::printf( "[DEBUG::%s::%s::%d] " text, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__ );

  #define PRINT_WIN32_ERROR( win32api )                                                       \
    {                                                                                         \
      char* message = imperium::win32::lookup_win32_error( NtLastError() );                   \
      if ( message ) {                                                                        \
        PRINTF_ERROR( "%s failed with error: %s (%ld)\n", win32api, message, NtLastError() ); \
        imperium::win32::call< fnLocalFree >( H_FUNC( "kernel32!LocalFree" ), message );      \
      } else                                                                                  \
        PRINTF_ERROR( "%s failed with error: %ld\n", win32api, NtLastError() );               \
    }

  #define PRINT_NT_ERROR( ntapi, status )                                                \
    {                                                                                    \
      char* message = imperium::win32::lookup_nt_error( status );                        \
      if ( message ) {                                                                   \
        PRINTF_ERROR( "%s failed with error: %s (0x%08X)\n", ntapi, message, status );   \
        imperium::win32::call< fnLocalFree >( H_FUNC( "kernel32!LocalFree" ), message ); \
      } else                                                                             \
        PRINTF_ERROR( "%s failed with error: %ld\n", ntapi, NtLastError() );             \
    }
#endif

//
// string
//
#define INIT_ANSI_STRING( str ) \
  { .Length = sizeof( str ) - sizeof( char ), .MaximumLength = sizeof( str ), .Buffer = ( PSTR ) str }
#define INIT_UNICODE_STRING( wstr ) \
  { .Length = sizeof( wstr ) - sizeof( wchar_t ), .MaximumLength = sizeof( wstr ), .Buffer = ( PWSTR ) wstr }

#endif  // IMPERIUM_MACROS_H
