#include <cstddef>
#include <cstdint>
#include <imperium/crypto.h>
#include <imperium/defs.h>
#include <imperium/ldr.h>
#include <imperium/macros.h>

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
   *  address of the DLL base
   */
  declfn void* module( _In_ uint32_t hash ) {
    PLDR_DATA_TABLE_ENTRY data  = { 0 };
    PLIST_ENTRY           head  = { 0 };
    PLIST_ENTRY           entry = { 0 };

    head  = &NtCurrentPeb()->Ldr->InLoadOrderModuleList;
    entry = head->Flink;

    for ( ; head != entry; entry = entry->Flink ) {
      data = reinterpret_cast< PLDR_DATA_TABLE_ENTRY >( entry );

      if ( crypto::dbj2( data->BaseDllName.Buffer, data->BaseDllName.Length / 2, true ) == hash ) {
        return data->DllBase;
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
   * @param Function
   *  hash of the function to get the address of
   *
   * @return
   *  address of the function
   */
  declfn void* function( _In_ void* library, _In_ uint32_t function ) {
    void*                                   address    = { 0 };
    PIMAGE_NT_HEADERS                       nt_headers = { 0 };
    PIMAGE_DOS_HEADER                       dos_header = { 0 };
    buf_t< IMAGE_EXPORT_DIRECTORY, size_t > export_dir = { 0 };
    uint32_t*                               names      = { 0 };
    uint32_t*                               funcs      = { 0 };
    uint16_t*                               ordinals   = { 0 };
    char*                                   func_name  = { 0 };

    //
    // sanity check arguments
    //
    if ( ! library || ! function ) {
      return nullptr;
    }

    //
    // check headers are correct
    //
    dos_header = static_cast< PIMAGE_DOS_HEADER >( library );
    nt_headers =
        reinterpret_cast< PIMAGE_NT_HEADERS >( reinterpret_cast< uint64_t >( library ) + dos_header->e_lfanew );

    if ( dos_header->e_magic != IMAGE_DOS_SIGNATURE || nt_headers->Signature != IMAGE_NT_SIGNATURE ) {
      return nullptr;
    }

    //
    // parse the header export address table
    //
    export_dir.data = reinterpret_cast< PIMAGE_EXPORT_DIRECTORY >(
        reinterpret_cast< uint64_t >( library ) +
        nt_headers->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].VirtualAddress );
    export_dir.len = nt_headers->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].Size;

    names = reinterpret_cast< uint32_t* >( reinterpret_cast< uint64_t >( library ) + export_dir.data->AddressOfNames );
    funcs =
        reinterpret_cast< uint32_t* >( reinterpret_cast< uint64_t >( library ) + export_dir.data->AddressOfFunctions );
    ordinals = reinterpret_cast< uint16_t* >(
        reinterpret_cast< uint64_t >( library ) + export_dir.data->AddressOfNameOrdinals );

    //
    // iterate over export address table director
    //
    for ( uint32_t i = 0; i < export_dir.data->NumberOfNames; i++ ) {
      //
      // retrieve function name
      //
      func_name = reinterpret_cast< char* >( reinterpret_cast< uint64_t >( library ) + names[ i ] );

      //
      // hash function name from Iat and
      // check the function name is what we are searching for.
      // if not found keep searching.
      //
      if ( crypto::dbj2( func_name, static_cast< uint32_t >( -1 ), true ) != function ) {
        continue;
      }

      //
      // resolve function pointer
      //
      address = reinterpret_cast< void* >( reinterpret_cast< uint64_t >( library ) + funcs[ ordinals[ i ] ] );

      //
      // check if function is a forwarded function
      //
      if ( ( reinterpret_cast< uint64_t >( address ) >= reinterpret_cast< uint64_t >( export_dir.data ) ) &&
           ( reinterpret_cast< uint64_t >( address ) <
               reinterpret_cast< uint64_t >( export_dir.data ) + export_dir.len ) ) {
        //
        // TODO: add support for forwarded functions
        //
        __debugbreak();
      }

      break;
    }

    return address;
  }
}  // namespace imperium::ldr
