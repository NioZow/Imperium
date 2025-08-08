#ifndef IMPERIUM_UTIL_H
#define IMPERIUM_UTIL_H

#include <cstddef>
#include <imperium/defs.h>

namespace imperium::util::string {
  /*!
   * @brief
   *	convert a string to uppercase
   *
   * @param str
   *	buffer to convert to uppercase
   */
  template< typename T >
  inline void upper( _Inout_ T str ) {
    do {
      if ( *str >= 'a' && *str <= 'z' ) {
        *str -= 0x20;
      }
    } while ( *( ++str ) );
  }

  /*!
   * @brief
   *	convert a string to uppercase
   *
   * @param str
   *	buffer to convert to uppercase
   *
   * @param size
   *	size of the string
   */
  template< typename T >
  inline void upper( _Inout_ T str, _Inout_ size_t size ) {
    for ( int i = 0; i < size; i++ ) {
      if ( str[ i ] >= 'a' && str[ i ] <= 'z' ) str[ i ] -= 0x20;
    }
  }

  /*!
   * @brief
   *  compare 2 strings
   *
   * @tparam T
   *  type of the strings
   *
   * @param str1
   *  first string
   *
   * @param str2
   *  second string
   *
   * @return
   *  true if the strings are the same otherwise false
   */
  template< typename T >
  inline bool compare( IN T str1, IN T str2 ) {
    do {
      if ( *str1 != *str2 ) return FALSE;
    } while ( *( ++str1 ) && *( ++str2 ) );

    if ( ! *str1 ) ++str2;

    return *str1 == *str2;
  }

  /*!
   * @brief
   *  compare 2 strings
   *
   * @tparam T
   *  type of the strings
   *
   * @param str1
   *  first string
   *
   * @param str2
   *  second string
   *
   * @param len
   *  length of the strings
   *
   * @return
   *  true if the strings are the same otherwise false
   */
  template< typename T >
  inline bool compare( IN T str1, IN T str2, IN ULONG len ) {
    int i = 0;

    do {
      if ( *str1 != *str2 ) return FALSE;
    } while ( ++i < len && *( ++str1 ) && *( ++str2 ) );

    if ( ! *str1 ) ++str2;

    return *str1 == *str2;
  }

  /*!
   * @brief
   *  get the length of a string
   *
   * @tparam T
   *  type of the string
   *
   * @param str
   *  string to get the length of
   *
   * @return
   *  length of the string
   */
  template< typename T >
  inline size_t len( T str ) {
    size_t cnt = 0;

    do {
      cnt++;
    } while ( *( ++str ) );

    return cnt;
  }
}  // namespace imperium::util::string

#endif  // IMPERIUM_UTIL_H
