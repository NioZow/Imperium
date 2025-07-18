#include <imperium.h>

using namespace imperium;

/*
 * @brief
 *  common entry point to dll/exe/shellcode output
 *  so you just have to modify this function to have your code executed
 *  dll/exe/pic have their own true entrypoint who just call this function
 *
 * @param args
 *  user passed arguments
 *  WARNING: this is optional and may be `nullptr`
 *
 *  NOTE:
 * - By default, the DLL entrypoint does not pass arguments.
 * - By default, the shellcode entrypoint pass its first argument
 * - By default, the exe entrypoint passes wrapped argc and argv
 *
 *  WARNING: for shellcode, arguments are get through the pointer that you can pass using `NtCreateThread` as argument
 *  the user calling `NtCreateThread` is responsible for ensuring the right format of the arguments.
 *  E.g. the first argument should be the program name (or dummy if not used) to ensure compability with exe format.
 *  If the passed pointer points to memory you are not allowed to access and you redeference it, the program will
 *  probably crash.
 */
declfn void instance::start( _In_opt_ const args_t* args ) {
  IMPERIUM_INSTANCE

  //
  // call LoadLibraryA to have the needed module is our peb
  // wont be able to resolve their functions otherwise
  //
  if ( ! win32::call< fnLoadLibraryA >( H_FUNC( "kernel32!LoadLibraryA" ), "user32.dll" ) ) {
    return;
  }

  win32::call< fnMessageBoxA >( H_FUNC( "user32!MessageBoxA" ), nullptr, ENC_STRING( "HackThePlanet" ),
      args && args->length > 0 ? args->arguments[ 0 ] : "Imperium", MB_OK );
}
