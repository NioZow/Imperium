#include <imperium.h>

extern "C" declfn void entry( _In_ void* args ) {
  imperium::instance::init();
  imperium::instance::start();
}
