#include <imperium/macros.h>
#include <imperium/defs.h>
#include <imperium/ldr.h>
#include <imperium/crypto.hpp>

namespace imperium::ldr {
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
            Data = reinterpret_cast<PLDR_DATA_TABLE_ENTRY>( Entry );

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
        DosHeader = static_cast<PIMAGE_DOS_HEADER>( Library );
        NtHeader  = reinterpret_cast<PIMAGE_NT_HEADERS>( U_PTR( Library ) + DosHeader->e_lfanew );

        if ( DosHeader->e_magic != IMAGE_DOS_SIGNATURE ||
                NtHeader->Signature != IMAGE_NT_SIGNATURE
        ) {
            return nullptr;
        }

        //
        // parse the header export address table
        //
        ExpDir     = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>( reinterpret_cast<ULONG_PTR>( Library ) + NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].VirtualAddress );
        ExpDirSize = NtHeader->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].Size;

        AddrNames  = reinterpret_cast<PDWORD>( reinterpret_cast<ULONG_PTR>(Library) + ExpDir->AddressOfNames );
        AddrFuncs  = reinterpret_cast<PDWORD>( reinterpret_cast<ULONG_PTR>(Library) + ExpDir->AddressOfFunctions );
        AddrOrdns  = reinterpret_cast<PWORD>( reinterpret_cast<ULONG_PTR>(Library) + ExpDir->AddressOfNameOrdinals );

        //
        // iterate over export address table director
        //
        for ( DWORD i = 0 ; i < ExpDir->NumberOfNames ; i++ ) {
            //
            // retrieve function name
            //
            FuncName = reinterpret_cast<PCHAR>( reinterpret_cast<ULONG_PTR>(Library) + AddrNames[ i ] );

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

