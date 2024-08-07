#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

//#include <Common.h>

#include <windows.h>
#include <winnt.h>
#include <ntstatus.h>
#include <core/Ldr.h>
#include <utility>
#include <core/Win32.h>
#include <crypto/Hash.h>
#include <common/Stardust.h>

//
// thread/process related macros
//
#define NtCurrentProcess()              ( ( HANDLE ) ( LONG_PTR ) ( -1 ) )
#define NtCurrentThread()               ( ( HANDLE ) ( LONG_PTR ) ( -2 ) )
#define NtCurrentProcessToken()         ( ( HANDLE ) ( LONG_PTR ) ( -4 ) )
#define NtCurrentThreadToken()          ( ( HANDLE ) ( LONG_PTR ) ( -5 ) )
#define NtCurrentThreadEffectiveToken() ( ( HANDLE ) ( LONG_PTR ) ( -6 ) )
#define NtLastError()                   ( NtCurrentTeb()->LastErrorValue  )
#define NtLastStatus()	                ( NtCurrentTeb()->LastStatusValue )
#define NtCurrentHeap()                 ( ( PVOID ) NtCurrentPeb()->ProcessHeap )
#define NtProcessHeap()                 NtCurrentHeap()
#define ZwCurrentProcess()              NtCurrentProcess()
#define ZwCurrentThread()               NtCurrentThread()
#define NtProcessImage()                ( PWCHAR ) NtCurrentPeb()->ProcessParameters->ImagePathName.Buffer
#define NtProcessCurrentDirectory()     ( PWCHAR ) NtCurrentPeb()->ProcessParameters->CurrentDirectory.DosPath

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
// compiler macros
//
#define CONSTEXPR     constexpr
#define CONSTEVAL     consteval
#define INLINE        inline
#define ALWAYS_INLINE [[gnu::always_inline]]
#define NO_INLINE     __attribute__ ((noinline))

//
// casting macros
//
#define C_PTR( x )   ( ( PVOID    ) ( x ) )
#define C_BYTE( x )  ( ( PBYTE    ) ( x ) )
#define U_PTR( x )   ( ( UINT_PTR ) ( x ) )
#define U_PTR32( x ) ( ( ULONG    ) ( x ) )
#define U_PTR64( x ) ( ( ULONG64  ) ( x ) )
#define A_PTR( x )   ( ( PCHAR    ) ( x ) )
#define W_PTR( x )   ( ( PWCHAR   ) ( x ) )

//
// dereference memory macros
//
#define C_DEF( x )   ( * ( PVOID* )  ( x ) )
#define C_DEF08( x ) ( * ( UINT8*  ) ( x ) )
#define C_DEF16( x ) ( * ( UINT16* ) ( x ) )
#define C_DEF32( x ) ( * ( UINT32* ) ( x ) )
#define C_DEF64( x ) ( * ( UINT64* ) ( x ) )

//
// NtStatus macros
//
#define NT_SUCCESS( NtStatus )     ( ( NTSTATUS ) ( NtStatus ) >= 0 )
#define NT_INFORMATION( NtStatus ) ( ( NTSTATUS ) ( NtStatus ) >> 30 == 1 )
#define NT_WARNING( NtStatus )     ( ( NTSTATUS ) ( NtStatus ) >> 30 == 2 )
#define NT_ERROR( NtStatus )       ( ( NTSTATUS ) ( NtStatus ) >> 30 == 3 )

//
// io macros
//
#define PRINTF( text, ... )             LogToConsole( text, ##__VA_ARGS__ );
#define PRINTF_DEBUG( text, ... )       PRINTF( "[DEBUG::%s::%s::%d] " text, __TIME__, __FUNCTION__, __LINE__, ##__VA_ARGS__ );
#define PRINTF_INFO( text, ... )        PRINTF( "[*] " text, ##__VA_ARGS__ );
#define PRINTF_ERROR( text, ... )       PRINTF_DEBUG( "[!] " text, ##__VA_ARGS__ );
#define PRINT_NT_ERROR( ntapi, status ) PRINTF_ERROR( "%s failed with error: 0x%08X\n", ntapi, status )
#define PRINT_WIN32_ERROR( win32api )   PRINTF_ERROR( "%s failed with error: %ld\n", win32api, NtLastError() )

//
// string
//
#define INIT_UNICODE_STRING( wstr ) { .Length = sizeof(wstr) - sizeof(WCHAR), .MaximumLength = sizeof(wstr), .Buffer = wstr }
#define INIT_ANSI_STRING( str )     { .Length = sizeof(str) - sizeof(CHAR), .MaximumLength = sizeof(str), .Buffer = str }

typedef struct _FUNCTION_HASH {
    // both are hashes
    ULONG Module;
    ULONG Function;
} FUNCTION_HASH, *PFUNCTION_HASH;

//
// functions
//
namespace Imperium {
    namespace win32 {
        /*!
         * @brief
         *  call a win32/nt function
         *
         * @tparam Func
         *  type of the function, e.g.
         *  typedef PVOID ( WINAPI*fnGetModuleHandleA )(
         *      PSTR Module
         *  );
         *
         * @tparam Args
         *  arguments to be passed to the function
         *  only the right number of arguments will be accepted
         *
         * @param FuncHash
         *  the hash of the dll and the hash of the func's name
         *
         * @param args
         *  args to pass to the function
         * @return
         */
        template<typename Func, class... Args>
        ALWAYS_INLINE auto call( FUNCTION_HASH FuncHash, Args... args ) {
            PVOID Module   = LdrModulePeb( FuncHash.Module );
            Func  Function = LdrFunctionAddr( Module, FuncHash.Function );

            return Function( std::forward< Args >( args )... );
        }
    }

    namespace crypto {
        template<typename T>
        ALWAYS_INLINE constexpr ULONG hash_string(
            IN T     String,
            IN ULONG Length
        ) {
            ULONG  Hash = { 0 };
            USHORT Char = { 0 };
            ULONG  Cnt  = { 0 };

            Hash = RANDOM_KEY;

            if ( ! String ) {
                return 0;
            }

            do {
                Char = *String;

                //
                // turn the character to uppercase
                //
                if ( Char >= 'a' && Char <= 'z' ) {
                    Char -= 0x20;
                }

                Hash = ( ( Hash << SEED ) + Hash ) + Char;
            } while ( ++Cnt < Length && *( ++String ) );

            return Hash;
        }
    }

    namespace mem {
        /*!
         * @brief
         *  custom memset implementation
         *  watch out if not specifying the type via the template it will default to int and not byte
         *  and make some mistakes along the way
         *
         * @param Out
         *  the output buffer
         *
         * @param In
         *  the input char
         *
         * @param Size
         *  the size of the buffer
         */
        template<typename T>
        ALWAYS_INLINE VOID set(
            PVOID Out,
            T     In,
            ULONG Size
        ) {
            for ( int i = 0 ; i < Size / sizeof( T ) ; i++ ) {
                ( ( T * ) Out )[ i ] = In;
            }
        }

        /*!
         * @brief
         *  custom memcopy implementation
         *
         * @param Out
         *  the output buffer
         *
         * @param In
         *  the input buffer
         *
         * @param Size
         *  the size of the buffer
         */
        INLINE VOID copy(
            PVOID Out,
            PVOID In,
            ULONG Size
        ) {
            for ( ULONG Cnt = 0 ; Cnt < Size ; Cnt++ ) {
                ( ( PBYTE ) Out )[ Cnt ] = ( ( PBYTE ) In )[ Cnt ];
            }
        }

        /*!
         * @brief
         *  fill a buffer with 0s
         *
         * @param Buffer
         *  the output buffer
         *
         * @param Size
         *  the size of the buffer
         */
        INLINE VOID zero(
            PVOID Buffer,
            ULONG Size
        ) {
            set< BYTE >( Buffer, 0, Size );
        }

        INLINE PVOID alloc(
            ULONG size
        ) {
            //win32::call< >()
            return NULL;
        }

        INLINE VOID free(
            PVOID ptr
        ) {
            return;
        }
    }
}

//
// consteval hashing
//
template<typename T>
consteval ULONG H_STR(
    IN T String
) {
    return Imperium::crypto::hash_string< T >( String, 0xFFFFFFFF );
}

consteval FUNCTION_HASH H_FUNC(
    PCSTR Symbol
) {
    PCSTR         Func       = Symbol;
    CHAR          Mod[ 100 ] = { 0 };
    FUNCTION_HASH FuncHash   = { 0 };
    ULONG         Size       = { 0 };
    INT           i          = { 0 };

    //
    // find the offset of the '!'
    //
    do {
        if ( *Func == '!' ) {
            //
            // copy the module in another variable
            // can't change just turn Function + 1 into a null byte because symbol is const
            // custom memcopy because compile time is more restrictive and does not like my casting
            // Imperium::mem::copy( Mod, Symbol, Func++ - Symbol );
            //
            Size = Func++ - Symbol;

            for ( ; i < Size ; i++ ) {
                Mod[ i ] = Symbol[ i ];
            }

            //
            // append .dll to the module
            //
            Mod[ i ]     = '.';
            Mod[ i + 1 ] = 'd';
            Mod[ i + 2 ] = 'l';
            Mod[ i + 3 ] = 'l';

            //
            // calculate the hashes
            //
            FuncHash.Function = H_STR( Func );
            FuncHash.Module   = H_STR( Mod );

            break;
        }
    } while ( *( ++Func ) );

    return FuncHash;
}

#endif //COMMON_DEFS_H
