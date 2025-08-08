#include <imperium.h>

#ifndef IMPERIUM_DLL_ENTRYPOINT
  #define IMPERIUM_DLL_ENTRYPOINT Imperium
#endif

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

using namespace imperium;

extern "C" __declspec( dllexport ) void IMPERIUM_DLL_ENTRYPOINT( _In_ const args_t* args ) {
  instance_t* instance = instance_t::init();
  instance->start( args );
  instance->clean();
}
