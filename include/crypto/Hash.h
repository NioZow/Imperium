#ifndef CRYPTO_HASH_H
#define CRYPTO_HASH_H

#include <Common.h>

template<typename T>
CONSTEXPR ULONG HashString(
    IN T     String,
    IN ULONG Length
) {
    ULONG  Hash = { 0 };
    USHORT Char = { 0 };
    ULONG  Cnt  = { 0 };

    Hash = H_MAGIC_KEY;

    if ( ! String ) {
        return 0;
    }

    do {
        Char = *String;

        //
        // turn the character to uppercase
        //
        if ( Char >= 'a' && Char <= 'z' ) {
            Char -= 0x20;
        }

        Hash = ( ( Hash << H_MAGIC_SEED ) + Hash ) + Char;
    } while ( ++Cnt < Length && *( ++String ) );

    return Hash;
}

template<typename T>
CONSTEVAL ULONG HASH_STR(
    IN T String
) {
    return HashString< T >( String, 0xFFFFFFFF );
}

#endif //CRYPTO_HASH_H
