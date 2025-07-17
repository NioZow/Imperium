#ifndef IMPERIUM_CRYPTO
#define IMPERIUM_CRYPTO

#include <concepts>
#include <imperium/defs.h>
#include <stdint.h>

//
// Cryptographic implementations are not safe !!!
// Do not use those in production
// Those are just enough to evade AV signatures
//

constexpr const uint32_t SEED = ( __LINE__ * 1000000 ) + ( __COUNTER__ * 1000 ) + ( ( __TIME__[ 7 ] - '0' ) * 100 ) +
                                ( ( __TIME__[ 6 ] - '0' ) * 10 ) + ( __TIME__[ 4 ] - '0' );

constexpr const uint32_t RANDOM_KEY = SEED % 0xFFFF;

namespace imperium::crypto {

  /*!
   * @brief
   *  hash a buffer
   *
   * @param buf
   *  buffer to hash
   *
   * @param len
   *  length of the buffer
   *  if the length is -1 then calculate the hash until a null byte is found
   *
   * @return
   *  hash
   */
  template< typename T >
  constexpr uint32_t dbj2( _In_ const T* buf, _In_ uint32_t len, _In_ bool to_upper = false ) {
    uint32_t hash = { 0 };
    uint8_t  tmp  = { 0 };
    uint32_t cnt  = { 0 };

    hash = RANDOM_KEY;

    if ( ! buf ) {
      return 0;
    }

    //
    // FIXME: the loop stops because of the *(++buffer) instead of length
    // and that causes problems when comparing strings with one having a defined length
    // but not the other, as they have different hash
    //
    do {
      tmp = *buf;

      //
      // turn the character to uppercase
      //
      if ( to_upper && tmp >= 'a' && tmp <= 'z' ) {
        tmp -= 0x20;
      }

      hash = ( ( hash << 5 ) + hash ) + tmp;
    } while ( ++cnt < len && ( len != static_cast< uint32_t >( -1 ) || *( ++buf ) ) );

    return hash;
  }
}  // namespace imperium::crypto


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
template< typename T >
consteval uint32_t H_STR( _In_ T* str ) {
  return imperium::crypto::dbj2( str, static_cast< uint32_t >( -1 ), true );
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
consteval SYMBOL_HASH H_FUNC( _In_ const char* Symbol ) {
  const char* Func       = Symbol;
  char        Mod[ 100 ] = { 0 };
  SYMBOL_HASH FuncHash   = { 0 };
  uint32_t    Size       = { 0 };
  uint32_t    i          = { 0 };

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

      for ( ; i < Size; i++ ) {
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

#endif  // IMPERIUM_CRYPTO
