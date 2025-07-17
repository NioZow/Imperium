#ifndef IMPERIUM_MEM
#define IMPERIUM_MEM

#include <cstdint>
#include <imperium/defs.h>

namespace imperium::mem {
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
  template< typename T >
  inline VOID set( void* Out, T In, uint32_t Size ) {
    for ( uint32_t i = 0; i < Size / sizeof( T ); i++ ) {
      ( ( T* ) Out )[ i ] = In;
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
  inline VOID copy( void* Out, void* In, uint32_t Size ) {
    for ( uint32_t Cnt = 0; Cnt < Size; Cnt++ ) {
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
  inline VOID zero( void* Buffer, uint32_t Size ) {
    set< unsigned char >( Buffer, 0, Size );
  }

  /*!
   * @brief
   *  allocate some memory from the heap
   *  wrapper for ntdll!RtlAllocateHeap
   *
   * @param Size
   *  number of bytes to allocate
   */
  PVOID alloc( uint32_t size );

  /*!
   * @brief
   *  ptr some memory from the heap
   *  wrapper for ntdll!RtlFreeHeap
   *
   * @param ptr
   *  pointer to memory that needs to be freed
   */
  VOID free( void* ptr );

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
  PVOID realloc( void* ptr, uint32_t size );
}  // namespace imperium::mem

#endif  // IMPERIUM_MEM
