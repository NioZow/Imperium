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
  _In_ ULONG ModuleHash
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
  _In_ PVOID Module,
  _In_ ULONG FunctionHash
);

/*!
 * @brief
 *  use the PEB to get the current directory
 * 
 * @return
 *  current directory
 */
PWCHAR LdrProcessPeb();

/*!
 * @brief
 *  get the path of the binary being executed by reading the PEB
 * 
 * @return 
 *  path
 */
LPWSTR LdrDirectoryPeb();

/*!
 * @brief
 *   use the PEB to get the PID of our process
 * 
 * @return 
 *   PID of the process ( NULL if unknown arch )
 */
DWORD LdrTid();

/*!
 * @brief
 *  use the PEB to get the TID of our process
 * 
 * @return
 *  TID ( NULL if unknown arch )
 */
DWORD LdrPid();

#endif //CORE_LDR_H
