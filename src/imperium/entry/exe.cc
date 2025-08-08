#include <imperium.h>

using namespace imperium;

int32_t main( _In_ int32_t argc, _In_ const char* argv[] ) {
  args_t args    = { 0 };
  args.arguments = argv;
  args.length    = argc;

  instance_t* instance = instance_t::init();
  instance->start( &args );
  instance->clean();
}
