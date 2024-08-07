#include <common/Imperium.h>

/*!
 * @brief
 *	convert a wide string to uppercase
 *
 * @param out
 *	out buffer
 *
 * @param sz
 *	size of the string (in/out)
 *
 * @param in
 *	size of the input string
 */
FUNC VOID StringToUpperW(
    OUT PWSTR out,
    IN INT    sz,
    IN PWSTR  in
) {
    for ( int i = 0 ; i < sz ; i++ ) {
        if ( in[ i ] >= 97 && in[ i ] <= 122 ) {
            out[ i ] = in[ i ] - 32;
        } else {
            out[ i ] = in[ i ];
        }
    }
}

/*!
 * @brief
 *	convert an ansi string to uppercase
 *
 * @param out
 *	out buffer
 *
 * @param sz
 *	size of the string (in/out)
 *
 * @param in
 *	size of the input string
 */
FUNC VOID StringToUpperA(
    OUT PCHAR out,
    IN INT    sz,
    IN PCHAR  in
) {
    for ( int i = 0 ; i < sz ; i++ ) {
        if ( in[ i ] >= 97 && in[ i ] <= 122 ) {
            out[ i ] = in[ i ] - 32;
        } else {
            out[ i ] = in[ i ];
        }
    }
}


FUNC INT StringCompareA( char *str1, char *str2 ) {
    do {
        if ( *str1 != *str2 ) return FALSE;
    } while ( *( ++str1 ) && *( ++str2 ) );

    if ( ! *str1 ) str2++;

    return *str1 == *str2;
}

FUNC INT StringNCompareA( char *str1, char *str2, int cnt ) {
    int i = 0;

    do {
        if ( *str1 != *str2 ) return 1;
    } while ( ++i < cnt && *( ++str1 ) && *( ++str2 ) );

    if ( ! *str1 ) str2++;

    return ( i == cnt ) ? 0 : *str1 != *str2;
}

FUNC int StringCompareW( PWCHAR str1, PWCHAR str2 ) {
    do {
        if ( *str1 != *str2 ) return FALSE;
    } while ( *( ++str1 ) && *( ++str2 ) );

    if ( ! *str1 ) str2++;

    return *str1 == *str2;
}

// disable optimisation otherwise it will call strlen and resolving will fail
// yeah weird, thanks @C5pider for figuring this out so quickly
#pragma GCC push_options
#pragma GCC optimize ("O0")

FUNC int StringLenA( char *str ) {
    int cnt = 0;

    do {
        cnt++;
    } while ( *( ++str ) );

    return cnt;
}

FUNC int StringLenW( wchar_t *str ) {
    int cnt = 0;

    do {
        cnt++;
    } while ( *( ++str ) );

    return cnt;
}

#pragma GCC pop_options
