#ifndef COMMON_COMMON_H
#define COMMMON_COMMON_H

#include <windows.h>

typedef PVOID ( WINAPI*fnGetModuleHandleA )(
    PSTR Module
);

#endif //COMMON_COMMON_H
