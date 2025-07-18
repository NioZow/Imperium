#include <cstdint>
#include <imperium.h>

int32_t main( _In_ int32_t argc, _In_ const char* argv[] ) {
  args_t args    = { 0 };
  args.arguments = argv;
  args.length    = argc;

  imperium::instance::init();
  imperium::instance::start( &args );
}
