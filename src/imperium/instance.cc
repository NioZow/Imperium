#include <cstdint>
#include <imperium/crypto.h>
#include <imperium/defs.h>
#include <imperium/ldr.h>
#include <imperium/macros.h>
#include <imperium/mem.h>

namespace imperium {
  /*!
   * @brief
   *  get a pointer to the instance by reading the peb
   *  this one in located in the process heaps table
   *
   * @return
   *  pointer to the instance
   */
  declfn instance_t* instance_t::find() {
    instance_t* instance = { 0 };

    for ( uint32_t i = 0; i < NtCurrentPeb()->NumberOfHeaps; i++ ) {
      instance = static_cast< instance_t* >( NtCurrentPeb()->ProcessHeaps[ i ] );
      if ( instance->context == 0xc0debabe ) break;
    }

    return instance;
  }

  /*!
   * @brief
   *  init the imperium instance
   *  you should init the instance and then start it.
   */
  declfn instance_t* instance_t::init() {
    instance_t**      instance   = { 0 };
    void*             ntdll      = { 0 };
    fnRtlAllocateHeap heap_alloc = { 0 };
    PPEB              peb        = NtCurrentPeb();

    //
    // check if there are enough heaps to hold our instance
    //
    if ( peb->NumberOfHeaps >= peb->MaximumNumberOfHeaps ) return nullptr;

    //
    // get the address of last heap to use to store a pointer to our instance
    //
    instance = reinterpret_cast< instance_t** >( &peb->ProcessHeaps[ peb->NumberOfHeaps++ ] );

    //
    // allocate memory for the instance
    // because of recursive issues it has to be allocated manually
    // can't use anything that ends up calling win32::call with is not ready
    // yet to be called
    //
    if ( ! ( ntdll = ldr::module( H_STR( "ntdll.dll" ) ) ) ) return nullptr;
    if ( ! ( heap_alloc =
                 reinterpret_cast< fnRtlAllocateHeap >( ldr::function( ntdll, H_STR( "RtlAllocateHeap" ) ) ) ) )
      return nullptr;

    if ( ! ( *instance =
                 static_cast< instance_t* >( heap_alloc( NtCurrentHeap(), HEAP_ZERO_MEMORY, sizeof( instance_t ) ) ) ) )
      return nullptr;

    //
    // cache RtlAllocateHeap address
    //
    ( *instance )->cached_modules.name            = H_STR( "ntdll.dll" );
    ( *instance )->cached_modules.address         = ntdll;
    ( *instance )->cached_modules.cached_function = static_cast< cached_function_t* >(
        heap_alloc( NtCurrentHeap(), HEAP_ZERO_MEMORY, sizeof( cached_function_t ) ) );
    ( *instance )->cached_modules.cached_function->address = reinterpret_cast< void* >( heap_alloc );
    ( *instance )->cached_modules.cached_function->name    = H_STR( "RtlAllocateHeap" );

    //
    // set a context to find the instance struct in memory
    //
    ( *instance )->context = 0xc0debabe;

    //
    // get the base address of the current implant in memory and the end.
    // subtract the implant end address with the start address you will
    // get the size of the implant in memory
    //
    ( *instance )->base.data = StRipStart();
    ( *instance )->base.len =
        reinterpret_cast< uint64_t >( StRipEnd() ) - reinterpret_cast< uint64_t >( ( *instance )->base.data );
    return ( *instance );
  }

  /*
   * @brief
   *  free up buffers and clean instance related stuff
   *  this function automatically gets called when you quit
   */
  declfn void instance_t::clean() {
    cached_module_t*   next_module     = &this->cached_modules;
    cached_function_t* next_function   = { 0 };
    cached_module_t*   cached_module   = { 0 };
    cached_function_t* cached_function = { 0 };
    fnRtlFreeHeap      heap_free =
        reinterpret_cast< fnRtlFreeHeap >( win32_t::resolve( H_FUNC( "ntdll!RtlFreeHeap" ) ).function_address );

    //
    // loop over all the cached modules
    // to free them
    //
    while ( ( cached_module = next_module ) ) {
      //
      // loop over all cached functions
      //
      next_function = cached_module->cached_function;
      while ( ( cached_function = next_function ) ) {
        //
        // free the function
        //
        next_function = cached_function->next;
        mem::zero( cached_function );
        heap_free( NtCurrentHeap(), 0, cached_function );
      }

      //
      // all functions from that module were freed
      // now free the module
      //
      next_module = cached_module->next;
      mem::zero( cached_module );
      if ( cached_module != &this->cached_modules ) heap_free( NtCurrentHeap(), 0, cached_module );
    }

    //
    // release the instance from its chains
    //
    mem::zero( this );
    heap_free( NtCurrentHeap(), 0, this );
  }
}  // namespace imperium
