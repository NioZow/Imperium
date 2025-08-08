#ifndef IMPERIUM_MEM_H
#define IMPERIUM_MEM_H

#include <cstddef>
#include <cstdint>
#include <imperium/crypto.h>
#include <imperium/defs.h>
#include <imperium/macros.h>
#include <imperium/win32.h>

#ifdef IMPERIUM_MEMLEAK
  #include <stdio.h>
#endif

/* Custom memory related functions
 *
 * WARNING:
 * Those functions are not the same as those from the std library.
 * Those use template and so length are different.
 * If you use those functions the same way you use the std library,
 * you will probably end up with some big bugs.
 */
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
   * @param len
   *  the length of the buffer
   */
  template< typename T = uint8_t >
  constexpr uint32_t set( _Out_ T* out, _In_ const T in, _In_ size_t len ) {
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
   * @param size
   *  the size of the buffer
   */
  template< typename T = uint8_t >
  constexpr uint32_t copy( _Out_ T* out, _In_ const T* in, _In_ size_t size ) {
    uint32_t cnt = 0;
    for ( ; cnt < size; cnt++ ) out[ cnt ] = in[ cnt ];
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
  template< typename T = uint8_t >
  constexpr uint32_t zero( _Out_ T* buf, _In_ size_t len = 1 ) {
    return set( buf, static_cast< T >( 0 ), len );
  }

  /*
   * @brief
   *  search a pattern in memory
   *
   * @param ptr
   *  buffer to search the pattern in
   *  WARNING: the `len` member might get synched
   *
   * @param pattern
   *  pattern to seek
   *  WARNING: the `len` member might get synched
   *
   * @return
   *  start of the FIRST occurence of the pattern
   */
  template< typename T1, typename T2 >
  T1* search( _Inout_ buf_t< T1, T2 >* ptr, _Inout_ buf_t< T1, T2 >* pattern ) {
    //
    // sanity check
    //
    if ( ! ptr || ! pattern ) return nullptr;

    //
    // sync `size` and `len`
    //
    ptr->sync_len_from_size();
    pattern->sync_len_from_size();

    for ( size_t i = 0; i <= ptr->len - pattern->len; ++i ) {
      bool match = true;

      for ( size_t j = 0; j < pattern->len; ++j ) {
        if ( ptr->data[ i + j ] != pattern->data[ j ] ) {
          match = false;
          break;
        }
      }

      if ( match ) return ptr->data + i;
    }

    return nullptr;
  }

#ifdef IMPERIUM_MEMLEAK
  #define alloc( size )        alloc_impl( size, __FILE__, __FUNCTION__, __LINE__ )
  #define free( ptr )          free_impl( ptr, __FILE__, __FUNCTION__, __LINE__ )
  #define realloc( ptr, size ) realloc_impl( ptr, size, __FILE__, __FUNCTION__, __LINE__ )

  inline void* alloc_impl( size_t size, const char* file, const char* func, int line ) {
    void* ptr =
        win32::call< fnRtlAllocateHeap >( H_FUNC( "ntdll!RtlAllocateHeap" ), NtProcessHeap(), HEAP_ZERO_MEMORY, size );
    printf( "alloc to 0x%08X at %s:%s:%d\n", ptr, file, func, line );
    return ptr;
  }

  inline void free_impl( void* ptr, const char* file, const char* func, int line ) {
    win32::call< fnRtlFreeHeap >( H_FUNC( "ntdll!RtlFreeHeap" ), NtProcessHeap(), 0, ptr );
    printf( "free from 0x%08X at %s:%s:%d\n", ptr, file, func, line );
  }

  inline void* realloc_impl( void* ptr, size_t size, const char* file, const char* func, int line ) {
    void* new_ptr = win32::call< fnRtlReAllocateHeap >( H_FUNC( "ntdll!RtlReAllocateHeap" ), NtProcessHeap(),
        HEAP_ZERO_MEMORY, ptr, size );
    printf( "realloc to 0x%08X from 0x%08X at %s:%s:%d\n", new_ptr, ptr, file, func, line );
    return ptr;
  }
#else
  /*!
   * @brief
   *  allocate some memory from the heap
   *  wrapper for ntdll!RtlAllocateHeap
   *
   * @param size
   *  number of bytes to allocate
   */
  inline void* alloc( size_t size ) {
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
  inline void free( _Inout_ void* ptr ) {
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
  inline void* realloc( _Inout_ void* ptr, _In_ size_t size ) {
    return win32::call< fnRtlReAllocateHeap >( H_FUNC( "ntdll!RtlReAllocateHeap" ), NtProcessHeap(), HEAP_ZERO_MEMORY,
        ptr, size );
  }
#endif

  /*!
   * @brief
   *  zero out memory, free it
   *  and zero out your data struct
   *
   * @param buf
   *  buffer to free
   */
  template< typename T1, typename T2 >
  inline void zfree( _Inout_ buf_t< T1, T2 >* buffer ) {
    buffer->sync_len_from_size();
    mem::zero( buffer->data, buffer->len );
    mem::free( buffer->data );
    mem::zero( buffer );
  }

}  // namespace imperium::mem

#endif  // IMPERIUM_MEM_H
