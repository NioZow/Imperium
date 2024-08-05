#ifndef CORE_UTILS_H
#define CORE_UTILS_H

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
VOID MmCopy(
    PVOID Out,
    PVOID In,
    ULONG Size
);

/*!
 * @brief
 *  custom memset implementation
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
template<typename T = BYTE>
FUNC VOID MmSet(
    PVOID Out,
    T     In,
    ULONG Size
) {
    if ( Size % sizeof( T ) == 0 ) {
        for ( int i = 0 ; i < Size ; i++ ) {
            ( ( T * ) Out )[ i ] = In;
        }
    }

    return;
}

#define MmZero( Buffer, Size ) MmSet<>( Buffer, 0, Size )

#endif //CORE_UTILS_H
