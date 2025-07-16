#include <imperium/crypto.hpp>
#include <imperium/defs.h>
#include <imperium/macros.h>
#include <imperium/win32.h>

namespace imperium::mem {
  /*!
   * @brief
   *  allocate some memory from the heap
   *  wrapper for ntdll!RtlAllocateHeap
   *
   * @param Size
   *  number of bytes to allocate
   */
  FUNC PVOID alloc( ULONG size ) {
    return win32::call< fnRtlAllocateHeap >(
        H_FUNC( "ntdll!RtlAllocateHeap" ), NtProcessHeap(), HEAP_ZERO_MEMORY, size );
  }

  /*!
   * @brief
   *  ptr some memory from the heap
   *  wrapper for ntdll!RtlFreeHeap
   *
   * @param ptr
   *  pointer to memory that needs to be freed
   */
  FUNC VOID free( PVOID ptr ) {
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
  FUNC PVOID realloc( PVOID ptr, ULONG size ) {
    win32::call< fnRtlReAllocateHeap >(
        H_FUNC( "ntdll!RtlReAllocateHeap" ), NtProcessHeap(), HEAP_ZERO_MEMORY, ptr, size );
  }
} // namespace imperium::mem
