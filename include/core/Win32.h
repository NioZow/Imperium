#ifndef WIN32_H
#define WIN32_H

#include <Common.h>

/*!
 * take from havoc, credits go to 5pider
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
VOID LogToConsole(
    IN PCSTR fmt,
    ...
);

#endif //WIN32_H
