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
    VOID printf(
        IN PCSTR fmt,
        ...
    );
}
