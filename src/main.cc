#include "imperium/crypto.h"
#include "imperium/io.h"
#include "imperium/macros.h"

#include <imperium.h>
#include <stdio.h>

namespace imperium::instance {
  declfn void start() {
    IMPERIUM_INSTANCE

    //
    // call LoadLibraryA to have the needed module is our peb
    // wont be able to resolve their functions otherwise
    //
    if ( ! win32::call< fnLoadLibraryA >( H_FUNC( "kernel32!LoadLibraryA" ), "user32.dll" ) ) {
      return;
    }

    win32::call< fnMessageBoxA >( H_FUNC( "user32!MessageBoxA" ), nullptr, "HackThePlanet", "Imperium", MB_OK );
  }
}  // namespace imperium::instance
