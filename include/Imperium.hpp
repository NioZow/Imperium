#ifndef IMPERIUM_H
#define IMPERIUM_H

//
// system headers
//
#include <windows.h>
#include <winnt.h>
#include <ntstatus.h>
#include <utility>

//
// project headers
//
#include <common/Common.h>
#include <common/Native.h>

//
// stardust related macros
//
#define IMPERIUM_INSTANCE PINSTANCE __LocalInstance = Imperium::instance::get();
#define Instance()        ( ( PINSTANCE ) ( __LocalInstance ) )
VOID Main();

#ifdef IMPERIUM_SHELLCODE
#define IMPERIUM_MAIN EXTERN_C FUNC INT PreMain()
#define D_SEC( x )  __attribute__( ( section( ".text$" #x "" ) ) )
#define FUNC        D_SEC( B )
#define ST_GLOBAL   __attribute__( ( section( ".global" ) ) )
#define ST_READONLY __attribute__( ( section( ".rdata" ) ) )
EXTERN_C PVOID StRipStart();
EXTERN_C PVOID StRipEnd();
#elif defined( IMPERIUM_EXE )
#define IMPERIUM_MAIN INT main()
#define FUNC
#define ST_GLOBAL
#define ST_READONLY
#define StRipStart() nullptr
#define StRipEnd()   nullptr
#elif defined( IMPERIUM_BOF )
#define IMPERIUM_MAIN INT main()
#define FUNC EXTERN_C
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
// status macros
//
#define SUCCESS( Status ) ( Status == 0 )

//
// io macros
//
#define PRINTF( text, ... )             Imperium::io::printf( text, ##__VA_ARGS__ )
#define PRINTF_INFO( text, ... )        PRINTF( "[*] " text "\n", ##__VA_ARGS__ )
#define PRINTF_ERROR( text, ... )       PRINTF( "[!] " text "\n", ##__VA_ARGS__ )
#define PRINT_NT_ERROR( ntapi, status ) PRINTF_ERROR( "%s failed with error: 0x%08X\n", ntapi, status )
#define PRINT_WIN32_ERROR( win32api )   PRINTF_ERROR( "%s failed with error: %ld\n", win32api, NtLastError() )

#ifdef IMPERIUM_DEBUG
#define PRINTF_DEBUG( text, ... )       PRINTF( "[DEBUG::%s::%s::%d] " text "\n", __TIME__, __FUNCTION__, __LINE__, ##__VA_ARGS__ )
#else
#define PRINTF_DEBUG( text, ... )
#endif

//
// string
//
#define INIT_ANSI_STRING( str )     { .Length = sizeof( str ) - sizeof( CHAR ), .MaximumLength = sizeof( str ), .Buffer = str }
#define INIT_UNICODE_STRING( wstr ) { .Length = sizeof( wstr ) - sizeof( WCHAR ), .MaximumLength = sizeof( wstr ), .Buffer = wstr }

//
// extern functions
//
/*!
 * @brief
 *  perform indirect syscall
 *
 * @param ...
 *  parameters of the syscall
 *
 * @return
 *  return status of the syscall
 */
EXTERN_C NTSTATUS SyscallIndirect(
    IN OUT OPTIONAL ... // args
);

/*!
 * @brief
 *  perform direct syscall
 *
 * @param ...
 *  parameters of the syscall
 *
 * @return
 *  return status of the syscall
 */
EXTERN_C NTSTATUS SyscallDirect(
    IN OUT OPTIONAL ... // args
);

//
// enum definition
//
typedef enum _WIN32_RESOLVE_FLAGS : ULONG {
    SymbolSyscall      = 0x01,
    SyscallAddInstance = 0x02,
} WIN32_RESOLVE_FLAGS, *PWIN32_RESOLVE_FLAGS;

//
// structure definition
//
typedef struct _SYMBOL_HASH {
    ULONG Module;   // module hash
    ULONG Function; // function hash
} SYMBOL_HASH, *PSYMBOL_HASH;

typedef struct _BUFFER {
    PVOID Buffer;
    ULONG Length;
} BUFFER, *PBUFFER;

typedef struct _SYSCALL {
    PVOID  Address;
    USHORT Ssn;
} SYSCALL, *PSYSCALL;

typedef struct _SYMBOL {
    union {
        PVOID Address;

        SYSCALL Syscall;
    };

    ULONG           ModuleHash;
    ULONG           FunctionHash;
    struct _SYMBOL *Next;
} SYMBOL, *PSYMBOL;

typedef struct _INSTANCE {
    //
    // context to find our instance in memory
    //
    ULONG Context;

    //
    // syscall structure for the current syscall to be executed
    //
    PSYSCALL Syscall;

    //
    // store already loaded functions
    //
    PSYMBOL Symbol;

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
/*!
 * @brief
 *  hash a string at compile time
 *
 * @tparam T
 *  type of the string
 *
 * @param str
 *  string to hash
 *
 * @return
 *  hash of the string
 */
template<typename T>
CONSTEVAL ULONG H_STR(
    IN T str
) {
    ULONG  Hash = { 0 };
    USHORT Char = { 0 };

    Hash = RANDOM_KEY;

    if ( ! str ) {
        return 0;
    }

    do {
        Char = *str;

        //
        // turn the character to uppercase
        //
        if ( Char >= 'a' && Char <= 'z' ) {
            Char -= 0x20;
        }

        Hash = ( ( Hash << SEED ) + Hash ) + Char;
    } while ( *( ++str ) );

    return Hash;
}

/*!
 * @brief
 *  get the hashed function & module name
 *  of a string at compile time
 *  fmt: user32!MessageBoxA
 *
 * @param Symbol
 *  string to hash
 *
 * @return
 *  hash of the function & module
 */
CONSTEVAL SYMBOL_HASH H_FUNC(
    PCSTR Symbol
) {
    PCSTR       Func       = Symbol;
    CHAR        Mod[ 100 ] = { 0 };
    SYMBOL_HASH FuncHash   = { 0 };
    ULONG       Size       = { 0 };
    INT         i          = { 0 };

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
// forward declarations and inline functions
//
namespace Imperium {

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

    namespace instance {
        /*!
         * @brief
         *  get a pointer to the instance by reading the peb
         *  this one in located in the process heaps table
         *
         * @return
         *  pointer to the instance
         */
        PINSTANCE get();

        namespace symbol {
            /*!
             * @brief
             *  store the function to load only once
             *
             * @param SymHash
             *  hashes of the func
             *
             * @param SymAddr
             *  addr of the module/function
             *
             * @param Ssn
             *  ssn of the syscall
             */
            PSYMBOL add(
                SYMBOL_HASH SymHash,
                PVOID       SymAddr,
                USHORT      Ssn = 0
            );

            PSYMBOL get(
                PSYMBOL_HASH SymbolHash
            );
        }
    }

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
         *  get the address of a function
         *
         * @param SymHash
         *  hashed symbol of the func
         *
         * @param Flags
         *  flags
         *
         * @return
         *  symbol
         */
        PSYMBOL resolve(
            SYMBOL_HASH SymHash,
            ULONG       Flags
        );

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
         * @param SymHash
         *  the hash of the dll and the hash of the func's name
         *
         * @param args
         *  args to pass to the function
         *
         * @return
         *  return value of win32 call
         */
        template<typename Func, class... Args>
        ALWAYS_INLINE auto call(
            SYMBOL_HASH SymHash,
            Args...     args
        ) {
            //
            // resolving using win32::resolve does not work there
            // can't figure out, i suspected this was a recursive issue
            // with calling mem::alloc but in my test it still crash
            //
            PVOID Module   = ldr::module( SymHash.Module );
            Func  Function = ldr::function( Module, SymHash.Function );

            return Function( std::forward< Args >( args )... );
            /*
            PSYMBOL Sym = { 0 };

            //
            // resolve the func
            //
            if ( ! ( Sym = resolve( SymHash, 0 ) ) ) {
                //
                // failed to resolve the function
                // no proper error handling for now
                //
                //PRINTF_ERROR( "Failed to resolve symbol with module 0x%08X and function 0x%08X", SymHash.Module, SymHash.Function );
                __debugbreak();
            }

            return ( ( Func ) Sym->Address )( std::forward< Args >( args )... );
            */
        }
    }

    namespace crypto {
        /*!
         * @brief
         *  hash a string
         *
         * @param String
         *  string to hash
         *
         * @param Length
         *  length of the string
         *
         * @return
         *  hash
         */
        ULONG hash_string(
            IN PCWSTR String,
            IN ULONG  Length
        );

        /*!
         * @brief
         *  hash a string
         *
         * @param String
         *  string to hash
         *
         * @param Length
         *  length of the string
         *
         * @return
         *  hash
         */
        ULONG hash_string(
            IN PCSTR String,
            IN ULONG Length
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
        PVOID alloc(
            ULONG size
        );

        /*!
         * @brief
         *  ptr some memory from the heap
         *  wrapper for ntdll!RtlFreeHeap
         *
         * @param ptr
         *  pointer to memory that needs to be freed
         */
        VOID free(
            PVOID ptr
        );

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
        PVOID realloc(
            PVOID ptr,
            ULONG size
        );
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
            SYMBOL_HASH SyscallHash,
            PSYSCALL    Syscall
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
        ALWAYS_INLINE NTSTATUS call(
            SYMBOL_HASH FuncHash,
            Args...     args
        ) {
            return win32::call< Func >( FuncHash, args... );
        }

        /*!
         * @brief
         *  perform indirect syscall
         *
         * @tparam Func
         *  type of the function
         *
         * @tparam Args
         *  arguments to be passed to the function
         *  only the right number of arguments will be accepted
         *
         * @param SymHash
         *  the hash of the dll and the hash of the func's name
         *
         * @param args
         *  args to pass to the function
         *
         * @return
         *  return value of syscall
         */
        template<typename Func, class... Args>
        ALWAYS_INLINE NTSTATUS indirect(
            SYMBOL_HASH SymHash,
            Args...     args
        ) {
            //
            // resolve the syscall
            //
            if ( ! win32::resolve( SymHash, SymbolSyscall | SyscallAddInstance ) ) {
                PRINTF_ERROR( "Failed to resolve symbol with module 0x%08X and function 0x%08X", SymHash.Module, SymHash.Function );
                return STATUS_INTERNAL_ERROR;
            }

            //
            // perform indirect syscall
            //
            return SyscallIndirect( std::forward< Args >( args )... );
        }

        /*!
         * @brief
         *  perform direct syscall
         *
         * @tparam Func
         *  type of the function
         *
         * @tparam Args
         *  arguments to be passed to the function
         *  only the right number of arguments will be accepted
         *
         * @param SymHash
         *  the hash of the dll and the hash of the func's name
         *
         * @param args
         *  args to pass to the function
         *
         * @return
         *  return value of syscall
         */
        template<typename Func, class... Args>
        ALWAYS_INLINE NTSTATUS direct(
            SYMBOL_HASH SymHash,
            Args...     args
        ) {
            //
            // resolve the syscall
            //
            if ( ! win32::resolve( SymHash, SymbolSyscall | SyscallAddInstance ) ) {
                PRINTF_ERROR( "Failed to resolve symbol with module 0x%08X and function 0x%08X", SymHash.Module, SymHash.Function );
                return STATUS_INTERNAL_ERROR;
            }

            //
            // perform direct syscall
            //
            return SyscallDirect( std::forward< Args >( args )... );
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
}

//
// functions code
//
#ifdef IMPERIUM
namespace Imperium {

    namespace instance {
        /*!
         * @brief
         *  get a pointer to the instance by reading the peb
         *  this one in located in the process heaps table
         *
         * @return
         *  pointer to the instance
         */
        FUNC PINSTANCE get() {
            PINSTANCE Instance = { 0 };
            ULONG     Context  = { 0 };

            for ( int i = 0 ; i < NtCurrentPeb()->NumberOfHeaps ; i++ ) {
                Context = ( ( PINSTANCE ) ( NtCurrentPeb()->ProcessHeaps[ i ] ) )->Context;

                if ( Context == 0xc0debabe ) {
                    Instance = NtCurrentPeb()->ProcessHeaps[ i ];
                    break;
                }
            }

            return Instance;
        }

        namespace symbol {
            /*!
             * @brief
             *  get a pointer to a function info if it is already stored in memory
             *
             * @return
             *  function address struct
             */
            FUNC PSYMBOL get(
                PSYMBOL_HASH FuncHash
            ) {
                IMPERIUM_INSTANCE

                PSYMBOL FuncAddr = Instance()->Symbol;

                //
                // if there is no function loaded quit now
                //
                if ( ! Instance()->Symbol ) {
                    return NULL;
                }

                //
                // iterate through all loaded functions
                //
                do {
                    //
                    // search if a function has the same hashes and so is the same
                    //
                    if ( FuncAddr->FunctionHash == FuncHash->Function &&
                         FuncAddr->ModuleHash == FuncHash->Module
                    ) {
                        break;
                    }
                } while ( ( FuncAddr = FuncAddr->Next ) );

                return FuncAddr;
            }

            /*!
             * @brief
             *  store the function to load only once
             *
             * @param SymHash
             *  hashes of the func
             *
             * @param SymAddr
             *  addr of the module/function
             *
             * @param Ssn
             *  ssn of the syscall
             */
            FUNC PSYMBOL add(
                SYMBOL_HASH SymHash,
                PVOID       SymAddr,
                USHORT      Ssn = 0
            ) {
                IMPERIUM_INSTANCE

                PSYMBOL *         Sym    = &Instance()->Symbol;
                PVOID             Module = { 0 };
                fnRtlAllocateHeap Func   = { 0 };

                //
                // get the address of the last symbol
                //
                while ( *Sym && ( Sym = &( *Sym )->Next ) );

                //
                // allocate the mem manually
                // cant call mem:alloc cuz it calls win32::call
                // creates a recursive infinite loop
                // todo: find a better way to do this?
                // does not fix anything still does not work
                //
                if ( ! ( Module = ldr::module( H_STR( "ntdll.dll" ) ) ) ) {
                    return nullptr;
                }

                if ( ! ( Func = ldr::function( Module, H_STR( "RtlAllocateHeap" ) ) ) ) {
                    return nullptr;
                }

                *Sym = Func( NtCurrentHeap(), HEAP_ZERO_MEMORY, sizeof( SYMBOL ) );

                //
                // set the symbol
                //
                ( *Sym )->FunctionHash = SymHash.Function;
                ( *Sym )->ModuleHash   = SymHash.Module;
                ( *Sym )->Address      = SymAddr;
                ( *Sym )->Syscall.Ssn  = Ssn;

                return *Sym;
            }
        }
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
        FUNC VOID printf(
            IN PCSTR fmt,
            ...
        ) {
            IMPERIUM_INSTANCE

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
            if ( ! Instance()->ConsoleOutput ) {
                win32::call< fnAttachConsole >( H_FUNC( "kernel32!AttachConsole" ), ATTACH_PARENT_PROCESS );

                if ( ! ( Instance()->ConsoleOutput = win32::call< fnGetStdHandle >( H_FUNC( "kernel32!GetStdHandle" ), STD_OUTPUT_HANDLE ) ) ) {
                    return;
                }
            }

            va_start( VaListArg, fmt );

            //
            // allocate space for the final string
            //
            OutputSize   = win32::call< fnVsnprintf >( H_FUNC( "msvcrt!vsnprintf" ), NULL, 0, fmt, VaListArg ) + 1;
            OutputString = mem::alloc( OutputSize );

            //
            // write the final string
            //
            win32::call< fnVsnprintf >( H_FUNC( "msvcrt!vsnprintf" ), OutputString, OutputSize, fmt, VaListArg );

            //
            // write it to the console
            //
            win32::call< fnWriteConsoleA >( H_FUNC( "kernel32!WriteConsoleA" ), Instance()->ConsoleOutput, OutputString, OutputSize, NULL, NULL );

            //
            // free the string
            //
            mem::zero( OutputString, OutputSize );
            mem::free( OutputString );

            va_end( VaListArg );
        }
    }

    namespace win32 {
        /*!
         * @brief
         *  get the address of a function
         *
         * @param SymHash
         *  hashed symbol of the func
         *
         * @param Flags
         *  flags
         *
         * @return
         *  symbol
         */
        FUNC PSYMBOL resolve(
            SYMBOL_HASH SymHash,
            ULONG       Flags
        ) {
            IMPERIUM_INSTANCE

            NTSTATUS NtStatus = { 0 };
            PSYMBOL  Sym      = { 0 };
            PVOID    Module   = { 0 };
            SYSCALL  Func     = { 0 };

            //
            // check if the function had already been resolved before
            //
            if ( ! ( Sym = instance::symbol::get( &SymHash ) ) ||
                 ( ! Sym->Syscall.Ssn && Flags & SymbolSyscall )
            ) {
                //
                // the function has not been resolved before
                //
                if ( Flags & SymbolSyscall ) {
                    //
                    // resolve the syscall
                    //
                    if ( ! NT_SUCCESS( NtStatus = syscall::resolve( SymHash, &Func ) ) ) {
                        return nullptr;
                    }
                } else {
                    //
                    // resolve the library from the peb
                    //
                    if ( ! ( Module = ldr::module( SymHash.Module ) ) ) {
                        //
                        // if it fails call LoadLibraryA (can't for now string hashed)
                        // todo: add support for that instead of loading the library to have it in peb
                        //
                        return nullptr;
                    }

                    //
                    // resolve the func
                    //
                    if ( ! ( Func.Address = ldr::function( Module, SymHash.Function ) ) ) {
                        //
                        // failed to resolve the func
                        //
                        return nullptr;
                    }
                }

                //
                // store the function for later use
                //
                if ( ! ( Sym = instance::symbol::add( SymHash, Func.Address, Func.Ssn ) ) ) {
                    return nullptr;
                }
            }

            //
            // add the syscall to the instance
            //
            if
            ( Flags & SymbolSyscall && Flags & SyscallAddInstance ) {
                Instance()->Syscall = &Sym->Syscall;
            }

            return Sym;
        }
    }

    namespace crypto {
        /*!
         * @brief
         *  hash a string
         *
         * @param String
         *  string to hash
         *
         * @param Length
         *  length of the string
         *
         * @return
         *  hash
         */
        FUNC ULONG hash_string(
            IN PCWSTR String,
            IN ULONG  Length
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

        /*!
         * @brief
         *  hash a string
         *
         * @param String
         *  string to hash
         *
         * @param Length
         *  length of the string
         *
         * @return
         *  hash
         */
        FUNC ULONG hash_string(
            IN PCSTR String,
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
        FUNC PVOID module(
            IN ULONG Hash
        ) {
            PLDR_DATA_TABLE_ENTRY Data  = { 0 };
            PLIST_ENTRY           Head  = { 0 };
            PLIST_ENTRY           Entry = { 0 };

            Head  = &NtCurrentPeb()->Ldr->InLoadOrderModuleList;
            Entry = Head->Flink;

            for ( ; Head != Entry ; Entry = Entry->Flink ) {
                Data = C_PTR( Entry );

                if ( crypto::hash_string( Data->BaseDllName.Buffer, Data->BaseDllName.Length / 2 ) == Hash ) {
                    return Data->DllBase;
                }
            }

            return nullptr;
        }

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
        FUNC PVOID function(
            IN PVOID Library,
            IN ULONG Function
        ) {
            PVOID                   Address    = { 0 };
            PIMAGE_NT_HEADERS       NtHeader   = { 0 };
            PIMAGE_DOS_HEADER       DosHeader  = { 0 };
            PIMAGE_EXPORT_DIRECTORY ExpDir     = { 0 };
            SIZE_T                  ExpDirSize = { 0 };
            PDWORD                  AddrNames  = { 0 };
            PDWORD                  AddrFuncs  = { 0 };
            PWORD                   AddrOrdns  = { 0 };
            PCHAR                   FuncName   = { 0 };

            //
            // sanity check arguments
            //
            if ( ! Library || ! Function ) {
                return nullptr;
            }

            //
            // check headers are correct
            //
            DosHeader = C_PTR( Library );
            NtHeader  = C_PTR( U_PTR( Library ) + DosHeader->e_lfanew );

            if ( DosHeader->e_magic != IMAGE_DOS_SIGNATURE ||
                 NtHeader->Signature != IMAGE_NT_SIGNATURE
            ) {
                return nullptr;
            }

            //
            // parse the header export address table
            //
            ExpDir     = C_PTR( Library + NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].VirtualAddress );
            ExpDirSize = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].Size;
            AddrNames  = C_PTR( Library + ExpDir->AddressOfNames );
            AddrFuncs  = C_PTR( Library + ExpDir->AddressOfFunctions );
            AddrOrdns  = C_PTR( Library + ExpDir->AddressOfNameOrdinals );

            //
            // iterate over export address table director
            //
            for ( DWORD i = 0 ; i < ExpDir->NumberOfNames ; i++ ) {
                //
                // retrieve function name
                //
                FuncName = C_PTR( U_PTR( Library ) + AddrNames[ i ] );

                //
                // hash function name from Iat and
                // check the function name is what we are searching for.
                // if not found keep searching.
                //
                if ( crypto::hash_string( FuncName, 0xFFFFFFFF ) != Function ) {
                    continue;
                }

                //
                // resolve function pointer
                //
                Address = C_PTR( U_PTR( Library ) + AddrFuncs[ AddrOrdns[ i ] ] );

                //
                // check if function is a forwarded function
                //
                if ( ( U_PTR( Address ) >= U_PTR( ExpDir ) ) &&
                     ( U_PTR( Address ) < U_PTR( ExpDir ) + ExpDirSize )
                ) {
                    //
                    // todo: add support for forwarded functions
                    //
                    __debugbreak();
                }

                break;
            }

            return Address;
        }
    }

    namespace mem {
        /*!
         * @brief
         *  allocate some memory from the heap
         *  wrapper for ntdll!RtlAllocateHeap
         *
         * @param Size
         *  number of bytes to allocate
         */
        FUNC PVOID alloc(
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
        FUNC VOID free(
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
        FUNC PVOID realloc(
            PVOID ptr,
            ULONG size
        ) {
            win32::call< fnRtlReAllocateHeap >(
                H_FUNC( "ntdll!RtlReAllocateHeap" ),
                NtProcessHeap(), HEAP_ZERO_MEMORY, ptr, size
            );
        }
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
        FUNC NTSTATUS resolve(
            IN SYMBOL_HASH SyscallHash,
            OUT PSYSCALL   Syscall
        ) {
            PBYTE SyscallAddr      = { 0 };
            PBYTE FirstSyscallAddr = { 0 };
            PVOID Ntdll            = { 0 };

            //
            // sanity check
            //
            if ( ! Syscall || ! SyscallHash.Function || ! SyscallHash.Module ) {
                return STATUS_INVALID_PARAMETER;
            }

            //
            // check ntdll address
            //
            if ( ! ( Ntdll = ldr::module( SyscallHash.Module ) ) ) {
                return STATUS_INTERNAL_ERROR;
            }

            //
            // get the address of the first syscall and the one we want to resolve
            //
            if ( ! ( SyscallAddr      = ldr::function( Ntdll, SyscallHash.Function ) ) ||
                 ! ( FirstSyscallAddr = ldr::function( Ntdll, H_STR( "NtAccessCheck" ) ) )
            ) {
                return STATUS_INTERNAL_ERROR;
            }

            //
            // make sure we got the address of the first syscall
            // in case NtAccessCheck is no longer the first syscall
            // might break because of NtQuerySystemTime
            // look for the syscall & ret instruction
            // as long as we find some it means we aren't at the first syscall
            //
            while ( *( FirstSyscallAddr - 0x0E ) == 0x0F &&
                    *( FirstSyscallAddr - 0x0D ) == 0x05 &&
                    *( FirstSyscallAddr - 0x0C ) == 0xC3
            ) {
                FirstSyscallAddr -= 32;
            }

            //
            // calculate the SSN
            // and add the address
            //
            Syscall->Address = SyscallAddr;
            Syscall->Ssn     = ( SyscallAddr - FirstSyscallAddr ) / 32;

            //
            // handle the case of ntdll!NtQuerySystemTime
            // it use to be a system call but no longer is
            // so its syscall stub is just a jmp instruction and is not 32 bytes
            // kinda of mess all offset from there
            //
            if ( ( SyscallAddr - FirstSyscallAddr ) % 32 != 0 ) {
                Syscall->Ssn++;
            }

            return STATUS_SUCCESS;
        }
    }
}
#endif //IMPERIUM

#endif //IMPERIUM_H
