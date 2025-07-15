#include <imperium.hpp>

FUNC INT Main(
    IN INT argc,
    IN PCSTR argv[]
) {
    IMPERIUM_INSTANCE

    //
    // call LoadLibraryA to have the needed module is our peb
    // wont be able to resolve their functions otherwise
    //
    if ( ! imperium::win32::call< fnLoadLibraryA >( H_FUNC( "kernel32!LoadLibraryA" ), "user32.dll" ) ) {
        return EXIT_FAILURE;
    }

    imperium::win32::call< fnMessageBoxA >( H_FUNC( "user32!MessageBoxA" ), nullptr, "Happy Hacking!", "imperium", MB_OK );
}
