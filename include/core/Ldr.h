#ifndef CORE_LDR_H
#define CORE_LDR_H

#include <windows.h>

/*!
 * @brief
 *  get the address of a module
 * 
 * @param ModuleHash
 *  hash of the module to get
 * 
 * @return
 *  address of the DLL base ( NULL if not found )
 */
PVOID LdrModulePeb(
    IN ULONG ModuleHash
);

/*!
 * @brief
 *  load the address of a function from base DLL address
 * 
 * @param Module 
 *  base address of the DLL
 * 
 * @param FunctionHash
 *  hash of the function to get the address of
 * 
 * @return
 *  address of the function ( NULL if not found )
 */
PVOID LdrFunctionAddr(
    IN PVOID Module,
    IN ULONG FunctionHash
);

/*!
 * @brief
 *  get the path of the binary being executed by reading the PEB
 *
 * @return
 *  path
 */
PWCHAR LdrProcessPeb();

/*!
 * @brief
 *  use the PEB to get the current directory
 *
 * @return
 *  current directory
 */
LPWSTR LdrDirectoryPeb();

#endif //CORE_LDR_H
