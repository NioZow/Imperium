#include <imperium/crypto.hpp>
#include <imperium/macros.h>

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
  FUNC ULONG hash_string( IN PCWSTR String, IN ULONG Length ) {
    ULONG  Hash = { 0 };
    USHORT Char = { 0 };
    ULONG  Cnt  = { 0 };

    Hash = RANDOM_KEY;

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

      Hash = ( ( Hash << SEED ) + Hash ) + Char;
    } while ( ++Cnt < Length && *( ++String ) );

    return Hash;
  }

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
  FUNC ULONG hash_string( IN PCSTR String, IN ULONG Length ) {
    ULONG  Hash = { 0 };
    USHORT Char = { 0 };
    ULONG  Cnt  = { 0 };

    Hash = RANDOM_KEY;

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

      Hash = ( ( Hash << SEED ) + Hash ) + Char;
    } while ( ++Cnt < Length && *( ++String ) );

    return Hash;
  }
}  // namespace imperium::crypto
