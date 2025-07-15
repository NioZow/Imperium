#include <imperium.hpp>

/*!
 * @brief
 *  main function put your code here
 *
 * @param Param
 *  parameters
 */
FUNC VOID Main(
    IN PVOID Params
) {
    IMPERIUM_INSTANCE

    //
    // call LoadLibraryA to have the needed module is our peb
    // wont be able to resolve their functions otherwise
    //
    if ( ! imperium::win32::call< fnLoadLibraryA >( H_FUNC( "kernel32!LoadLibraryA" ), "user32.dll" ) ) {
        return;
    }

    imperium::win32::call< fnMessageBoxA >( H_FUNC( "user32!MessageBoxA" ), nullptr, "Happy Hacking!", "Imperium", MB_OK );
}


