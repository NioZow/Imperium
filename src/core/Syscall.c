#include <Stardust.h>
#include <core/Syscall.h>

#ifdef INDIRECT_SYSCALL
/*!
 * @brief
 *  resolve syscall information (SSN, address...)
 *
 * @param Hash
 *  hash of the syscall name
 *
 * @return
 *  pointer to a data structure containing information about the syscall
 */
PSYSCALL SyscallResolve(
  _In_ ULONG Hash
) {
  DWORD    Cnt     = { 0 };
  PBYTE    Address = { 0 };
  PSYSCALL Syscall = { 0 };
 
  // Get the address of the syscall
  if ( ! ( Address = ( PBYTE )LdrFunctionAddr( Instance.Modules.Ntdll, Hash ) ) ) {
    return NULL;
  }

  // Search the SSN of the syscall using Hell's gate technique
  while ( TRUE ) {
    
    // reached ret or syscall instructions
    // missed the SSN then
    if ( *( Address + Cnt ) == 0xC3 || // reached ret
       ( *( Address + Cnt ) == 0x0F && *( Address + Cnt + 1 ) == 0x05 ) // reached syscall
    ) {
      return NULL;
    }

    // opcodes should be :
    // mov r10, rcx
    // mov eax, <ssn>
    if ( *( Address + Cnt     ) == 0x4C &&
         *( Address + Cnt + 1 ) == 0x8B &&
         *( Address + Cnt + 2 ) == 0xD1 &&
         *( Address + Cnt + 3 ) == 0xB8 &&
         *( Address + Cnt + 6 ) == 0x00 &&
         *( Address + Cnt + 7 ) == 0x00
    ) { 

      // Allocate memory for SYSCALL structure
      if ( ! ( Syscall = ( PSYSCALL )Instance.Win32.LocalAlloc( LPTR, sizeof( SYSCALL ) ) ) ){
        return NULL;
      }
      
      Syscall->Address = Address;
      Syscall->Ssn     = *( Address + 4 + Cnt ) + *( Address + 5 + Cnt );

      return Syscall;
    } 

    Cnt++;
  }

  return NULL;
}

#endif //INDIRECT_SYSCALL
