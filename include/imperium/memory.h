#include <imperium/defs.h>

namespace imperium::mem {
    #pragma GCC push_options
    #pragma GCC optimize ("O0")
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
    inline VOID set(
        PVOID Out,
        T     In,
        ULONG Size
    ) {
        for ( int i = 0 ; i < Size / sizeof( T ) ; i++ ) {
            ( ( T * ) Out )[ i ] = In;
        }
    }
    #pragma GCC pop_options

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
    inline VOID copy(
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
    inline VOID zero(
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
