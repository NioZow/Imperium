#include <common/Imperium.h>

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

                if ( Imperium::crypto::hash_string( Data->BaseDllName.Buffer, Data->BaseDllName.Length / 2 ) == Hash ) {
                    return Data->DllBase;
                }
            }

            return NULL;
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
                return NULL;
            }

            //
            // check headers are correct
            //
            DosHeader = C_PTR( Library );
            NtHeader  = C_PTR( U_PTR( Library ) + DosHeader->e_lfanew );

            if ( DosHeader->e_magic != IMAGE_DOS_SIGNATURE ||
                 NtHeader->Signature != IMAGE_NT_SIGNATURE
            ) {
                return NULL;
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
                if ( Imperium::crypto::hash_string( FuncName, 0xFFFFFFFF ) != Function ) {
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
            STARDUST_INSTANCE

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
                Imperium::win32::call< fnAttachConsole >( H_FUNC( "kernel32!AttachConsole" ), ATTACH_PARENT_PROCESS );

                if ( ! ( Instance()->ConsoleOutput = Imperium::win32::call< fnGetStdHandle >( H_FUNC( "kernel32!GetStdHandle" ), STD_OUTPUT_HANDLE ) ) ) {
                    return;
                }
            }

            va_start( VaListArg, fmt );

            //
            // allocate space for the final string
            //
            OutputSize   = Imperium::win32::call< fnVsnprintf >( H_FUNC( "msvcrt!vsnprintf" ), NULL, 0, fmt, VaListArg ) + 1;
            OutputString = Imperium::mem::alloc( OutputSize );

            //
            // write the final string
            //
            Imperium::win32::call< fnVsnprintf >( H_FUNC( "msvcrt!vsnprintf" ), OutputString, OutputSize, fmt, VaListArg );

            //
            // write it to the console
            //
            Imperium::win32::call< fnWriteConsoleA >( H_FUNC( "kernel32!WriteConsoleA" ), Instance()->ConsoleOutput, OutputString, OutputSize, NULL, NULL );

            //
            // free the string
            //
            Imperium::mem::zero( OutputString, OutputSize );
            Imperium::mem::free( OutputString );

            va_end( VaListArg );
        }
    }
}
