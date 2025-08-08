#include <imperium/defs.h>
#include <imperium/macros.h>
#include <imperium/syscall.h>

namespace imperium {
  /*!
   * @brief
   *  get the address of a function
   *
   * @param symbol
   *  hashed symbol of the module and func
   *
   * @return
   *  symbol
   */
  declfn win32_t win32_t::resolve( symbol_t symbol ) {
    instance_t*        instance        = instance_t::find();
    win32_t            win32           = { 0 };
    void*              mod             = { 0 };
    cached_module_t*   cached_module   = &instance->cached_modules;
    cached_function_t* cached_function = { 0 };
    fnRtlAllocateHeap  heap_alloc =
        reinterpret_cast< fnRtlAllocateHeap >( instance->cached_modules.cached_function->address );

    //
    // check if the function was already resolved and cached
    //
    if ( cached_module ) {
      do {
        //
        // check if functions from this module have already been cached
        //
        if ( cached_module->name == symbol.module ) {
          //
          // try to find that cached function
          // now that we have found the module
          //
          cached_function = cached_module->cached_function;
          if ( cached_function ) {
            do {
              //
              // check if we have already resolved that function
              // if so, return its address
              //
              if ( cached_function->name == symbol.function ) {
                win32.function_address = cached_function->address;
                goto clean;
              }
            } while ( cached_function->next && ( cached_function = cached_function->next ) );
          }

          //
          // the module was cached but not the function
          // read the cached module address
          //
          mod = cached_module->address;
          break;
        }
      } while ( cached_module->next && ( cached_module = cached_module->next ) );
    }


    //
    // NOTE:
    // we gonna need to allocate memory for the caching
    // however mem::alloc calls win32::call which calls the current func
    // this creates an infinite loop
    // so we call RtlAllocateHeap directly
    // RtlAllocateHeap address has already been cached and its address
    // is in the heap_alloc variable
    //

    //
    // check if we get the module via caching
    //
    if ( ! mod ) {
      //
      // we did not get the module via caching
      // let's try to resolve it
      //
      if ( ! ( mod = ldr::module( symbol.module ) ) ) {
        //
        // the module failed to resolve
        // it means it is not in our PEB
        // means it should probably be resolved using LoadLibraryA instead
        // but wont resolve it there because we are using hashing
        // which is not obvisouly supported by LoadLibraryA
        //
        goto clean;
      }

      //
      // now that we found the module cache it
      //
      if ( cached_module ) {
        cached_module->next = static_cast< cached_module_t* >(
            heap_alloc( NtCurrentHeap(), HEAP_ZERO_MEMORY, sizeof( cached_module_t ) ) );
        cached_module = cached_module->next;
      } else {
        cached_module = static_cast< cached_module_t* >(
            heap_alloc( NtCurrentHeap(), HEAP_ZERO_MEMORY, sizeof( cached_module_t ) ) );
      }

      cached_module->name    = symbol.module;
      cached_module->address = mod;
    }

    //
    // resolve the function address
    //
    if ( ! ( win32.function_address = ldr::function( mod, symbol.function ) ) ) {
      //
      // failed to resolve the func
      //
      goto clean;
    }

    //
    // cache the function address
    //
    if ( cached_function ) {
      cached_function->next = static_cast< cached_function_t* >(
          heap_alloc( NtCurrentHeap(), HEAP_ZERO_MEMORY, sizeof( cached_function_t ) ) );
      cached_function = cached_function->next;
    } else
      cached_module->cached_function = cached_function = static_cast< cached_function_t* >(
          heap_alloc( NtCurrentHeap(), HEAP_ZERO_MEMORY, sizeof( cached_function_t ) ) );

    cached_function->name    = symbol.function;
    cached_function->address = win32.function_address;


  clean:
    return win32;
  }
}  // namespace imperium
