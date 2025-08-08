#include <imperium.h>

using namespace imperium;

extern "C" declfn void entry( _In_ const args_t* args ) {
  instance_t* instance = instance_t::init();
  instance->start( args );
  instance->clean();
}
