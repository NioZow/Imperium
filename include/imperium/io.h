#ifndef IMPERIUM_IO_H
#define IMPERIUM_IO_H

#include <cstdint>
#include <windows.h>

namespace imperium::io {
  /*!
   * took from havoc, credits go to 5pider
   *
   * @brief
   *  custom printf implementation
   *
   * @param fmt
   *  format of the string
   *
   * @param ...
   *  printf parameters
   */
  void printf( _In_ const char* fmt, ... );
  void print_bytes( uint8_t* data, size_t size );
  void print_hex( uint8_t* data, size_t size );
}  // namespace imperium::io

#endif  // IMPERIUM_IO_H
