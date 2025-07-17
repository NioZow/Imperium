#include <imperium.h>

BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved ) {
  switch ( fdwReason ) {
    case DLL_PROCESS_ATTACH:
      break;

    case DLL_THREAD_ATTACH:
      break;

    case DLL_THREAD_DETACH:
      break;

    case DLL_PROCESS_DETACH:
      break;
  }

  return TRUE;
}

extern "C" __declspec( dllexport ) void Imperium() {
  imperium::instance::init();
  imperium::instance::start();
}
