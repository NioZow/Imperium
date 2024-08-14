#include <Imperium.h>

/*!
 * @brief
 *  main function put your code here
 *
 * @param Param
 *  parameters
 */
FUNC VOID Main() {
    IMPERIUM_INSTANCE

    //
    // call LoadLibraryA to have the needed module is our peb
    // wont be able to resolve their functions otherwise
    //
    if ( ! Imperium::win32::call< fnLoadLibraryA >( H_FUNC( "kernel32!LoadLibraryA" ), "user32.dll" ) ) {
        return;
    }

    Imperium::win32::call< fnMessageBoxA >( H_FUNC( "user32!MessageBoxA" ), NULL, "Happy Hacking!", "Imperium", MB_OK );
}
