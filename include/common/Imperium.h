#ifndef IMPERIUM_H
#define IMPERIUM_H

//
// system headers
//
#include <windows.h>
#include <stdio.h>
#include <winnt.h>
#include <ntstatus.h>
#include <utility>

//
// project headers
//
#include <common/Stardust.h>
#include <common/Common.h>
#include <common/Native.h>
#include <core/Syscall.h>

//
// stardust instances
//
EXTERN_C ULONG __Instance_offset;
EXTERN_C PVOID __Instance;

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
// hashing macros
//
#define SEED 5
#define RANDOM_KEY 5381

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
#define PRINTF( text, ... )             Imperium::io::printf( text, ##__VA_ARGS__ );
#define PRINTF_DEBUG( text, ... )       PRINTF( "[DEBUG::%s::%s::%d] " text, __TIME__, __FUNCTION__, __LINE__, ##__VA_ARGS__ );
#define PRINTF_INFO( text, ... )        PRINTF( "[*] " text, ##__VA_ARGS__ );
#define PRINTF_ERROR( text, ... )       PRINTF_DEBUG( "[!] " text, ##__VA_ARGS__ );
#define PRINT_NT_ERROR( ntapi, status ) PRINTF_ERROR( "%s failed with error: 0x%08X\n", ntapi, status )
#define PRINT_WIN32_ERROR( win32api )   PRINTF_ERROR( "%s failed with error: %ld\n", win32api, NtLastError() )

//
// string
//
#define INIT_ANSI_STRING( str )     { .Length = sizeof( str ) - sizeof( CHAR ), .MaximumLength = sizeof( str ), .Buffer = str }
#define INIT_UNICODE_STRING( wstr ) { .Length = sizeof( wstr ) - sizeof( WCHAR ), .MaximumLength = sizeof( wstr ), .Buffer = wstr }

typedef struct _FUNCTION_HASH {
    ULONG Module;   // module hash
    ULONG Function; // function hash
} FUNCTION_HASH, *PFUNCTION_HASH;

typedef struct _BUFFER {
    PVOID Buffer;
    ULONG Length;
} BUFFER, *PBUFFER;

typedef struct _INSTANCE {
    PSYSCALL Syscall;

    //
    // base address and size
    // of the implant
    //
    BUFFER Base;

    HANDLE ConsoleOutput;
} INSTANCE, *PINSTANCE;

//
// consteval hashing
//
template<typename T>
consteval ULONG H_STR(
    IN T String
) {
    ULONG  Hash = { 0 };
    USHORT Char = { 0 };

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
    } while ( *( ++String ) );

    return Hash;
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

//
// functions
//
namespace Imperium {
    namespace ldr {
        /*!
         * 5pider implementation, credits go to him
         *
         * @brief
         *  get the address of a module
         *
         * @param ModuleHash
         *  hash of the module to get
         *
         * @return
         *  address of the DLL base ( NULL if not found )
         */
        PVOID module(
            IN ULONG Hash
        );

        /*!
         * 5pider implementation, credits go to him
         *
         * @brief
         *  load the address of a function from base DLL address
         *
         * @param Module
         *  base address of the DLL
         *
         * @param FunctionHash
         *  hash of the function to get the address of
         *
         * @return
         *  address of the function ( NULL if not found )
         */
        PVOID function(
            IN PVOID Library,
            IN ULONG Function
        );
    }

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
         *
         * @return
         *  return value of win32 call
         */
        template<typename Func, class... Args>
        ALWAYS_INLINE auto call( FUNCTION_HASH FuncHash, Args... args ) {
            PVOID Module   = ldr::module( FuncHash.Module );
            Func  Function = ldr::function( Module, FuncHash.Function );

            return Function( std::forward< Args >( args )... );
        }
    }

    namespace crypto {
        ULONG hash_string(
            IN PCWSTR String,
            IN ULONG  Length
        );

        ULONG hash_string(
            IN PCSTR String,
            IN ULONG Length
        );
    }

    namespace io {
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
        VOID printf(
            IN PCSTR fmt,
            ...
        );
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

        /*!
         * @brief
         *  allocate some memory from the heap
         *  wrapper for ntdll!RtlAllocateHeap
         *
         * @param Size
         *  number of bytes to allocate
         */
        INLINE PVOID alloc(
            ULONG size
        ) {
            return win32::call< fnRtlAllocateHeap >(
                H_FUNC( "ntdll!RtlAllocateHeap" ),
                NtProcessHeap(), HEAP_ZERO_MEMORY, size
            );
        }

        /*!
         * @brief
         *  ptr some memory from the heap
         *  wrapper for ntdll!RtlFreeHeap
         *
         * @param ptr
         *  pointer to memory that needs to be freed
         */
        INLINE VOID free(
            PVOID ptr
        ) {
            win32::call< fnRtlFreeHeap >(
                H_FUNC( "ntdll!RtlFreeHeap" ),
                NtProcessHeap(), 0, ptr
            );
        }

        /*!
         * @brief
         *  reallocate some memory from the heap
         *  wrapper for ntdll!RtlFreeHeap
         *
         * @param ptr
         *  allocated memory buffer
         *
         * @param size
         *  size to allocate
         *
         * @return
         *  pointer to the reallocated memory
         */
        INLINE PVOID realloc(
            PVOID ptr,
            ULONG size
        ) {
            return win32::call< fnRtlReAllocateHeap >(
                H_FUNC( "ntdll!RtlReAllocateHeap" ),
                NtProcessHeap(), HEAP_ZERO_MEMORY, ptr, size
            );
        }
    }

    namespace util::string {
        /*!
         * @brief
         *	convert a string to uppercase
         *
         * @param str
         *	buffer to convert to uppercase
         */
        template<typename T>
        ALWAYS_INLINE VOID upper(
            IN OUT T str
        ) {
            do {
                if ( *str >= 'a' && *str <= 'z' ) {
                    *str -= 0x20;
                }
            } while ( *( ++str ) );
        }

        /*!
         * @brief
         *	convert a string to uppercase
         *
         * @param str
         *	buffer to convert to uppercase
         *
         * @param size
         *	size of the string
         */
        template<typename T>
        ALWAYS_INLINE VOID upper(
            IN OUT T str,
            IN ULONG size
        ) {
            for ( int i = 0 ; i < size ; i++ ) {
                if ( str[ i ] >= 'a' && str[ i ] <= 'z' ) {
                    str[ i ] -= 0x20;
                }
            }
        }

        /*!
         * @brief
         *  compare 2 strings
         *
         * @tparam T
         *  type of the strings
         *
         * @param str1
         *  first string
         *
         * @param str2
         *  second string
         *
         * @return
         *  true if the strings are the same otherwise false
         */
        template<typename T>
        ALWAYS_INLINE BOOL compare(
            IN T str1,
            IN T str2
        ) {
            do {
                if ( *str1 != *str2 ) return FALSE;
            } while ( *( ++str1 ) && *( ++str2 ) );

            if ( ! *str1 ) ++str2;

            return *str1 == *str2;
        }

        /*!
         * @brief
         *  compare 2 strings
         *
         * @tparam T
         *  type of the strings
         *
         * @param str1
         *  first string
         *
         * @param str2
         *  second string
         *
         * @param len
         *  length of the strings
         *
         * @return
         *  true if the strings are the same otherwise false
         */
        template<typename T>
        ALWAYS_INLINE BOOL compare(
            IN T     str1,
            IN T     str2,
            IN ULONG len
        ) {
            int i = 0;

            do {
                if ( *str1 != *str2 ) return FALSE;
            } while ( ++i < len && *( ++str1 ) && *( ++str2 ) );

            if ( ! *str1 ) ++str2;

            return *str1 == *str2;
        }


#pragma GCC push_options
#pragma GCC optimize ("O0")
        /*!
         * the above disables optimisation otherwise it might call strlen
         * resolving will fail and the program will crash
         * thanks @C5pider for figuring this out so quickly
         *
         * @brief
         *  get the length of a string
         *
         * @tparam T
         *  type of the string
         *
         * @param str
         *  string to get the length of
         *
         * @return
         *  length of the string
         */
        template<typename T>
        ALWAYS_INLINE ULONG len(
            T str
        ) {
            int cnt = 0;

            do {
                cnt++;
            } while ( *( ++str ) );

            return cnt;
        }
#pragma GCC pop_options
    }

    namespace syscall {
        /*!
         * @brief
         *  resolve syscall information (SSN, address...)
         *
         * @param SyscallHash
         *  hash of the syscall
         *
         * @param Syscall
         *  struct that will receive the address and ssn of the syscall
         *
         * @return
         *  pointer to a data structure containing information about the syscall
         */
        NTSTATUS resolve(
            FUNCTION_HASH SyscallHash,
            PSYSCALL      Syscall
        );

        /*!
         * @brief
         *  call a nt function
         *  just forward the call to win32::call
         *
         * @tparam Func
         *  type of the function
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
         *
         * @return
         *  return value of syscall
         */
        template<typename Func, class... Args>
        ALWAYS_INLINE NTSTATUS call( FUNCTION_HASH FuncHash, Args... args ) {
            return win32::call< Func >( FuncHash, args... );
        }
    }
}

#endif //IMPERIUM_H
