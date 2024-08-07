#include <Common.h>
#include <core/Ldr.h>
#include <crypto/Hash.h>

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
FUNC PVOID LdrModulePeb(
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
 *  retrieve image header
 *
 * @param Image
 *  image base pointer to retrieve header from
 *
 * @return
 *  pointer to Nt Header
 */
FUNC PIMAGE_NT_HEADERS LdrpImageHeader(
    IN PVOID Image
) {
    PIMAGE_DOS_HEADER DosHeader = { 0 };
    PIMAGE_NT_HEADERS NtHeader  = { 0 };

    DosHeader = C_PTR( Image );

    if ( DosHeader->e_magic != IMAGE_DOS_SIGNATURE ) {
        return NULL;
    }

    NtHeader = C_PTR( U_PTR( Image ) + DosHeader->e_lfanew );

    if ( NtHeader->Signature != IMAGE_NT_SIGNATURE ) {
        return NULL;
    }

    return NtHeader;
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
FUNC PVOID LdrFunctionAddr(
    IN PVOID Library,
    IN ULONG Function
) {
    PVOID                   Address    = { 0 };
    PIMAGE_NT_HEADERS       NtHeader   = { 0 };
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
    // retrieve header of library
    //
    if ( ! ( NtHeader = LdrpImageHeader( Library ) ) ) {
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
