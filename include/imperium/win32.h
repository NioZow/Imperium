#ifndef IMPERIUM_WIN32_H
#define IMPERIUM_WIN32_H

#include <cstdint>
#include <imperium/crypto.h>
#include <imperium/defs.h>
#include <imperium/io.h>
#include <imperium/ldr.h>
#include <utility>

typedef DWORD( WINAPI* fnFormatMessageA )( _In_ DWORD dwFlags, _In_opt_ LPCVOID lpSource, _In_ DWORD dwMessageId,
    _In_ DWORD dwLanguageId, _Out_ LPTSTR lpBuffer, _In_ DWORD nSize, _In_opt_ va_list* Arguments );

typedef ULONG( WINAPI* fnRtlNtStatusToDosError )( _In_ NTSTATUS Status );

namespace imperium {

  struct win32_t {
    void* function_address;

    /*!
     * @brief
     *  get the address of a function
     *
     * @param symbol
     *  hashed symbol of the module and func
     *
     * @return
     *  address
     */
    static win32_t resolve( symbol_t symbol );

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
    template< typename Func, class... Args >
    inline auto exec( Args... args ) {
      if ( ! function_address ) __debugbreak();
      return reinterpret_cast< Func >( function_address )( std::forward< Args >( args )... );
    }
  };

  namespace win32 {
    /*
     * @brief
     *  resolve a function and execute it
     *
     * @tparam Func
     *  type of the function
     *
     * @tparam Args
     *  args to pass to the function
     *
     * @return
     *  function return value
     */
    template< typename Func, class... Args >
    inline auto call( symbol_t symbol, Args... args ) {
      return win32_t::resolve( symbol ).exec< Func >( std::forward< Args >( args )... );
    }

#ifndef IMPERIUM_RELEASE
    /*
     * @brief
     *  lookup a win32 error to get the associated message
     *
     * @param error_code
     *  error code to lookup
     *
     * @return
     *  error message
     *  this buffer must be freed using kernel32!LocalFree
     */
    inline char* lookup_win32_error( uint32_t error_code ) {
      buf_t< char > message = { 0 };

      //
      // lookup the error code
      //
      message.len = win32::call< fnFormatMessageA >( H_FUNC( "kernel32!FormatMessageA" ),
          FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
          error_code, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), reinterpret_cast< char* >( &message.data ), 0,
          nullptr );

      //
      // null terminate the string to remove some garbage
      //
      if ( message.data ) message.data[ message.len - 3 ] = 0;

      return message.data;
    }

    /*
     * @brief
     *  lookup a ntstatus error to get the associated message
     *
     * @param error_code
     *  error code to lookup
     *
     * @return
     *  error message
     *  this buffer must be freed using kernel32!LocalFree
     */
    inline char* lookup_nt_error( NTSTATUS status ) {
      uint32_t win32_error = win32::call< fnRtlNtStatusToDosError >( H_FUNC( "ntdll!RtlNtStatusToDosError" ), status );
      return lookup_win32_error( win32_error );
    }
#endif  // IMPERIUM_RELEASE
  }  // namespace win32
}  // namespace imperium

#endif  // IMPERIUM_WIN32_H
