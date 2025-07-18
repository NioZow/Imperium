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

// seed only used for dbj2
// because we then rely on dbj2 to create more seeds
constexpr const uint32_t SEED = ( __LINE__ * 1000000 ) + ( __COUNTER__ * 1000 ) + ( ( __TIME__[ 7 ] - '0' ) * 100 ) +
                                ( ( __TIME__[ 6 ] - '0' ) * 10 ) + ( __TIME__[ 4 ] - '0' );

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
    T        tmp  = { 0 };
    uint32_t cnt  = { 0 };

    hash = SEED;

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


/*
 * @brief
 *  try to generate a random seed
 *  this is cryptographically unsafe and shouldn't be used outside the scope of this project
 *  but hopefully distrituvity is good enough
 *
 *  INFO: setting this to consteval because you have much better options at runtime
 *  and random stuff is complicated at compile time
 *  btw this is AI generated and AI is not known to make secure code, another reason you
 *  shouldn't use this
 *
 *
 *  WARNING: should only be used for basic obfuscation against automatic detections like AV
 *
 * @return
 *  seed
 */
template< typename T >
consteval T get_initial_seed() {
  constexpr auto mix_seed = []( T a, T b, T c ) constexpr {
    constexpr int bits = sizeof( T ) * 8;
    a ^= b;
    a -= ( b << ( bits * 14 / 32 ) ) | ( b >> ( bits * 18 / 32 ) );
    b ^= c;
    b -= ( c << ( bits * 11 / 32 ) ) | ( c >> ( bits * 21 / 32 ) );
    c ^= a;
    c -= ( a << ( bits * 25 / 32 ) ) | ( a >> ( bits * 7 / 32 ) );
    a ^= b;
    a -= ( b << ( bits * 16 / 32 ) ) | ( b >> ( bits * 16 / 32 ) );
    b ^= c;
    b -= ( c << ( bits * 4 / 32 ) ) | ( c >> ( bits * 28 / 32 ) );
    c ^= a;
    c -= ( a << ( bits * 14 / 32 ) ) | ( a >> ( bits * 18 / 32 ) );
    return c;
  };

  return mix_seed( imperium::crypto::dbj2( __FILE__, static_cast< T >( -1 ), false ) ^ __LINE__,
      imperium::crypto::dbj2( __DATE__, static_cast< T >( -1 ), false ) ^
          imperium::crypto::dbj2( __TIME__, static_cast< T >( -1 ), false ),
      static_cast< T >( __COUNTER__ ) );
}

/*
 * @brief
 *  get a another seed from a seed
 *  predictive but prevents repeating a key in xor
 *
 *  WARNING: completely unsafe
 *
 * @param seed
 *  current seed
 *
 * @return
 *  new seed
 */
template< typename T >
constexpr T seed_next( _In_ T seed ) {
  return ( T( 1103515245 ) * seed + T( 12345 ) ) % ( T( 1 ) << ( sizeof( T ) * 8 - 1 ) );
}

// cipher structure
// seed is the key equivalent
// but alg is so weak i won't even call key
template< typename T1, typename T2, uint32_t N >
struct cipher_t {
  T1 data[ N ];
};

// encrypting with a simple xor
// no key repetition, get a new seed instead
template< typename T1, typename T2, size_t len >
constexpr auto encrypt( _In_ const T1 ( &buf )[ len ], _In_ T2 seed ) {
  auto blob = cipher_t< T1, T2, len > { {} };
  T1   mask = static_cast< T1 >( ~T1( 0 ) );

  for ( size_t i = 0; i < len; i++ ) {
    if ( i > 0 && i % sizeof( T2 ) == 0 ) seed = seed_next( seed );
    blob.data[ i ] = buf[ i ] ^ static_cast< T1 >( seed >> ( ( i % sizeof( T2 ) ) * 8 ) ) & mask;
  }

  return blob;
}

// can't return the string directly because its lifetime expires
// could use `static` to get around this
// but static links against `__cxa_guard_acquire` to ensure thread safety
// however this is not compatible with shellcode `-nostdlib` option
//
// HACK: throught `volatile` we ensure we are being decrypted at runtime
#define _ENC_STRING( STRING )                                                    \
  ( [ & ] {                                                                      \
    constexpr uint32_t seed         = get_initial_seed< uint32_t >();            \
    constexpr auto     blob         = encrypt< char, uint32_t >( STRING, seed ); \
    volatile uint32_t  runtime_seed = seed;                                      \
    return encrypt< char, uint32_t >( blob.data, runtime_seed );                 \
  }() )

#define ENC_STRING( var_name, string_literal )                      \
  const auto  var_name##_encrypted = _ENC_STRING( string_literal ); \
  const char* var_name             = var_name##_encrypted.data;

#endif  // IMPERIUM_CRYPTO
