#ifndef IMPERIUM_MACROS
#define IMPERIUM_MACROS

#include <imperium/io.h>

#define IMPERIUM_INSTANCE PINSTANCE __LocalInstance = imperium::instance::get();
#define Instance()        ( ( PINSTANCE ) ( __LocalInstance ) )

#ifdef IMPERIUM_SHELLCODE
  #define D_SEC( x )  __attribute__( ( section( ".text$" #x "" ) ) )
  #define FUNC        D_SEC( B )
  #define ST_GLOBAL   __attribute__( ( section( ".global" ) ) )
  #define ST_READONLY __attribute__( ( section( ".rdata" ) ) )
EXTERN_C PVOID StRipStart();
EXTERN_C PVOID StRipEnd();
#elif defined( IMPERIUM_EXE )
  #define FUNC
  #define ST_GLOBAL
  #define ST_READONLY
  #define StRipStart() nullptr
  #define StRipEnd()   nullptr
#elif defined( IMPERIUM_BOF )
  #define IMPERIUM_MAIN INT main()
  #define FUNC          EXTERN_C
  #define ST_GLOBAL
  #define ST_READONLY
  #define StRipStart() nullptr
  #define StRipEnd()   nullptr
#else
  #error Please define either IMPERIUM_SHELLCODE, IMPERIUM_EXE or IMPERIUM_BOF
#endif

//
// pseudo handles
//
#define NtCurrentProcess()              ( ( HANDLE ) ( LONG_PTR ) ( -1 ) )
#define NtCurrentThread()               ( ( HANDLE ) ( LONG_PTR ) ( -2 ) )
#define NtCurrentProcessToken()         ( ( HANDLE ) ( LONG_PTR ) ( -4 ) )
#define NtCurrentThreadToken()          ( ( HANDLE ) ( LONG_PTR ) ( -5 ) )
#define NtCurrentThreadEffectiveToken() ( ( HANDLE ) ( LONG_PTR ) ( -6 ) )

//
// peb/teb related macros
//
#define NtLastError()               ( NtCurrentTeb()->LastErrorValue )
#define NtLastStatus()              ( NtCurrentTeb()->LastStatusValue )
#define NtCurrentHeap()             ( ( PVOID ) NtCurrentPeb()->ProcessHeap )
#define NtProcessHeap()             NtCurrentHeap()
#define ZwCurrentProcess()          NtCurrentProcess()
#define ZwCurrentThread()           NtCurrentThread()
#define NtProcessImage()            ( PWCHAR ) NtCurrentPeb()->ProcessParameters->ImagePathName.Buffer
#define NtProcessCurrentDirectory() ( PWCHAR ) NtCurrentPeb()->ProcessParameters->CurrentDirectory.DosPath

#if _WIN64
  #define NtCurrentProcessId() ( ( DWORD ) ( __readgsdword( 0x40 ) ) )
#elif _WIN32
  #define NtCurrentProcessId() ( ( DWORD ) ( __readfsdword( 0x20 ) ) )
#endif

#if _WIN64
  #define NtCurrentThreadId() ( ( DWORD ) ( __readgsdword( 0x48 ) ) )
#elif _WIN32
  #define NtCurrentThreadId() ( ( DWORD ) ( __readgsdword( 0x24 ) ) )
#endif

//
// casting macros
//
#define C_PTR( x )   ( ( PVOID ) ( x ) )
#define C_BYTE( x )  ( ( PBYTE ) ( x ) )
#define U_PTR( x )   ( ( UINT_PTR ) ( x ) )
#define U_PTR32( x ) ( ( ULONG ) ( x ) )
#define U_PTR64( x ) ( ( ULONG64 ) ( x ) )
#define A_PTR( x )   ( ( PCHAR ) ( x ) )
#define W_PTR( x )   ( ( PWCHAR ) ( x ) )

//
// dereference memory macros
//
#define C_DEF( x )   ( *( PVOID* ) ( x ) )
#define C_DEF08( x ) ( *( UINT8* ) ( x ) )
#define C_DEF16( x ) ( *( UINT16* ) ( x ) )
#define C_DEF32( x ) ( *( UINT32* ) ( x ) )
#define C_DEF64( x ) ( *( UINT64* ) ( x ) )

//
// NtStatus macros
//
#define NT_SUCCESS( NtStatus )     ( ( NTSTATUS ) ( NtStatus ) >= 0 )
#define NT_INFORMATION( NtStatus ) ( ( NTSTATUS ) ( NtStatus ) >> 30 == 1 )
#define NT_WARNING( NtStatus )     ( ( NTSTATUS ) ( NtStatus ) >> 30 == 2 )
#define NT_ERROR( NtStatus )       ( ( NTSTATUS ) ( NtStatus ) >> 30 == 3 )

//
// status macros
//
#define SUCCESS( Status ) ( Status == 0 )

//
// io macros
//
#define PRINTF( text, ... )             imperium::io::printf( text, ##__VA_ARGS__ )
#define PRINTF_INFO( text, ... )        PRINTF( "[*] " text "\n", ##__VA_ARGS__ )
#define PRINTF_ERROR( text, ... )       PRINTF( "[!] " text "\n", ##__VA_ARGS__ )
#define PRINT_NT_ERROR( ntapi, status ) PRINTF_ERROR( "%s failed with error: 0x%08X\n", ntapi, status )
#define PRINT_WIN32_ERROR( win32api )   PRINTF_ERROR( "%s failed with error: %ld\n", win32api, NtLastError() )

#ifdef IMPERIUM_DEBUG
  #define PRINTF_DEBUG( text, ... ) \
    PRINTF( "[DEBUG::%s::%s::%d] " text "\n", __TIME__, __FUNCTION__, __LINE__, ##__VA_ARGS__ )
#else
  #define PRINTF_DEBUG( text, ... )
#endif

//
// string
//
#define INIT_ANSI_STRING( str ) \
  { .Length = sizeof( str ) - sizeof( CHAR ), .MaximumLength = sizeof( str ), .Buffer = str }
#define INIT_UNICODE_STRING( wstr ) \
  { .Length = sizeof( wstr ) - sizeof( WCHAR ), .MaximumLength = sizeof( wstr ), .Buffer = wstr }

#endif  // IMPERIUM_MACROS
