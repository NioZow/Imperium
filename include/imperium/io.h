#ifndef IMPERIUM_IO
#define IMPERIUM_IO

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
}  // namespace imperium::io

#endif  // IMPERIUM_IO
