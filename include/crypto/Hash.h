#ifndef CRYPTO_HASH_H
#define CRYPTO_HASH_H

#include <Common.h>

ULONG HashString(
    _In_ PVOID  String,
    _In_ SIZE_T Length
);

CONSTEXPR ULONG ExprHashStringA(
    _In_ PCHAR String
) {
    ULONG Hash = { 0 };
    CHAR  Char = { 0 };

    Hash = H_MAGIC_KEY;

    if ( ! String ) {
        return 0;
    }

    while ( ( Char = *String++ ) ) {
        /* turn current character to uppercase */
        if ( Char >= 'a' ) {
            Char -= 0x20;
        }

        Hash = ( ( Hash << H_MAGIC_SEED ) + Hash ) + Char;
    }

    return Hash;
}

CONSTEXPR ULONG ExprHashStringW(
    _In_ PVOID  String,
    _In_ SIZE_T Length
) {
    ULONG  Hash = { 0 };
    PUCHAR Ptr  = { 0 };
    UCHAR  Char = { 0 };

    if ( ! String ) {
        return 0;
    }

    Hash = H_MAGIC_KEY;
    Ptr  = ( ( PUCHAR ) String );

    do {
        Char = *Ptr;

        if ( ! Length ) {
            if ( ! *Ptr ) break;
        } else {
            if ( U_PTR( Ptr - U_PTR( String ) ) >= Length ) break;
            if ( !*Ptr ) ++Ptr;
        }

        if ( Char >= 'a' ) {
            Char -= 0x20;
        }

        Hash = ( ( Hash << 5 ) + Hash ) + Char;

        ++Ptr;
    } while ( TRUE );

    return Hash;
}

#define HASH_STR( x )  ExprHashStringA( ( x ) )
#define HASH_STRA( x ) ExprHashStringA( ( x ) )
#define HASH_STRW( x, y ) ExprHashStringW( x, y )

#endif // CRYPTO_HASH_H
