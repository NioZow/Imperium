#include <imperium.h>

extern "C" declfn void entry( _In_ const args_t* args ) {
  imperium::instance::init();
  imperium::instance::start( args );
}
