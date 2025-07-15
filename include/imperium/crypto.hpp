#include <imperium/defs.h>

#define SEED 5
#define RANDOM_KEY 5381

/*!
 * @brief
 *  hash a string at compile time
 *
 * @tparam T
 *  type of the string
 *
 * @param str
 *  string to hash
 *
 * @return
 *  hash of the string
 */
template<typename T>
consteval ULONG H_STR(
    IN T str
) {
    ULONG  Hash = { 0 };
    USHORT Char = { 0 };

    Hash = RANDOM_KEY;

    if ( ! str ) {
        return 0;
    }

    do {
        Char = *str;

        //
        // turn the character to uppercase
        //
        if ( Char >= 'a' && Char <= 'z' ) {
            Char -= 0x20;
        }

        Hash = ( ( Hash << SEED ) + Hash ) + Char;
    } while ( *( ++str ) );

    return Hash;
}

/*!
 * @brief
 *  get the hashed function & module name
 *  of a string at compile time
 *  fmt: user32!MessageBoxA
 *
 * @param Symbol
 *  string to hash
 *
 * @return
 *  hash of the function & module
 */
consteval SYMBOL_HASH H_FUNC(
    PCSTR Symbol
) {
    PCSTR       Func       = Symbol;
    CHAR        Mod[ 100 ] = { 0 };
    SYMBOL_HASH FuncHash   = { 0 };
    ULONG       Size       = { 0 };
    INT         i          = { 0 };

    //
    // find the offset of the '!'
    //
    do {
        if ( *Func == '!' ) {
            //
            // copy the module in another variable
            // can't change just turn Function + 1 into a null byte because symbol is const
            // custom memcopy because compile time is more restrictive and does not like my casting
            // imperium::mem::copy( Mod, Symbol, Func++ - Symbol );
            //
            Size = Func++ - Symbol;

            for ( ; i < Size ; i++ ) {
                Mod[ i ] = Symbol[ i ];
            }

            //
            // append .dll to the module
            //
            Mod[ i ]     = '.';
            Mod[ i + 1 ] = 'd';
            Mod[ i + 2 ] = 'l';
            Mod[ i + 3 ] = 'l';

            //
            // calculate the hashes
            //
            FuncHash.Function = H_STR( Func );
            FuncHash.Module   = H_STR( Mod );

            break;
        }
    } while ( *( ++Func ) );

    return FuncHash;
}

namespace imperium::crypto {
    /*!
        * @brief
        *  hash a string
        *
        * @param String
        *  string to hash
        *
        * @param Length
        *  length of the string
        *
        * @return
        *  hash
        */
    ULONG hash_string(
        IN PCWSTR String,
        IN ULONG  Length
    );

    /*!
        * @brief
        *  hash a string
        *
        * @param String
        *  string to hash
        *
        * @param Length
        *  length of the string
        *
        * @return
        *  hash
        */
    ULONG hash_string(
        IN PCSTR String,
        IN ULONG Length
    );
}
