#include <imperium.h>

int main( _In_ uint32_t argc, _In_ const char* argv[] ) {
  imperium::instance::init();
  imperium::instance::start();
}
