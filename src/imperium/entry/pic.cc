#include <imperium.hpp>

extern "C" FUNC void entry( _In_ void* args ) {
  imperium::instance::init();
  imperium::instance::start();
}
