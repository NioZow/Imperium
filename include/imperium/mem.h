#ifndef IMPERIUM_MEM
#define IMPERIUM_MEM

#include <cstdint>
#include <imperium/crypto.h>
#include <imperium/defs.h>
#include <imperium/win32.h>

namespace imperium::mem {
  /*!
   * @brief
   *  custom memset implementation
   *
   * @param out
   *  the output buffer
   *
   * @param in
   *  the input char
   *
   * @param size
   *  the length of the buffer
   */
  template< typename T >
  constexpr uint32_t set( _In_ T* out, _In_ const T in, _In_ uint32_t len = 1 ) {
    uint32_t cnt = 0;
    while ( cnt < len ) out[ cnt++ ] = in;
    return cnt;
  }

  /*!
   * @brief
   *  custom memcopy implementation
   *
   * @param out
   *  the output buffer
   *
   * @param in
   *  the input buffer
   *
   * @param len
   *  the length of the buffer
   */
  template< typename T >
  constexpr uint32_t copy( _Out_ T out, _In_ const T* in, _In_ uint32_t len ) {
    uint32_t cnt = 0;
    while ( cnt < len ) out[ cnt ] = in[ cnt++ ];
    return cnt;
  }

  /*!
   * @brief
   *  fill a buffer with 0s
   *
   * @param buf
   *  the output buffer
   *
   * @param len
   *  the length of the buffer
   */
  template< typename T >
  constexpr uint32_t zero( T* buf, uint32_t len = 1 ) {
    return set( buf, static_cast< T >( 0 ), len );
  }

  /*!
   * @brief
   *  allocate some memory from the heap
   *  wrapper for ntdll!RtlAllocateHeap
   *
   * @param size
   *  number of bytes to allocate
   */
  inline auto alloc( uint32_t size ) {
    return win32::call< fnRtlAllocateHeap >( H_FUNC( "ntdll!RtlAllocateHeap" ), NtProcessHeap(), HEAP_ZERO_MEMORY,
        size );
  }

  /*!
   * @brief
   *  ptr some memory from the heap
   *  wrapper for ntdll!RtlFreeHeap
   *
   * @param ptr
   *  pointer to memory that needs to be freed
   */
  inline void free( void* ptr ) {
    win32::call< fnRtlFreeHeap >( H_FUNC( "ntdll!RtlFreeHeap" ), NtProcessHeap(), 0, ptr );
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
  inline auto realloc( void* ptr, uint32_t size ) {
    win32::call< fnRtlReAllocateHeap >( H_FUNC( "ntdll!RtlReAllocateHeap" ), NtProcessHeap(), HEAP_ZERO_MEMORY, ptr,
        size );
  }
}  // namespace imperium::mem

#endif  // IMPERIUM_MEM
