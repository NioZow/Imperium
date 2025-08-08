#ifndef IMPERIUM_DEFS_H
#define IMPERIUM_DEFS_H

#include <cstdint>
#include <imperium/native.h>

struct symbol_t {
  uint32_t module;
  uint32_t function;
};

// cache function addresses to resolve them faster
// PERF: good but there no sorting based on most used functions
struct cached_function_t {
  uint32_t           name;
  void*              address;
  cached_function_t* next;
};

// cache module addresses to resolve them faster
// PERF: good but there no sorting based on most used modules
struct cached_module_t {
  uint32_t           name;
  void*              address;
  cached_function_t* cached_function;
  cached_module_t*   next;
};

//
// WARNING:
// The caller does not need to have both `len` and `size` members
// accordly. This is the callee role, to synchronize those using
// `sync_from_len` or `sync_from_size`. This is also the callee
// role to synchronize those members when it returns a buf_t
// struct.
//
template< typename T1 = uint8_t, typename T2 = unsigned long >
struct buf_t {
  T1* data;

  //
  // lenght of the data
  // I define length as the size relative to a T1 size
  // And so basically the number of T1 elements
  //
  // WARNING: this is not AUTOMICALLY synched with `size` :(
  // You have to call `sync_len_from_size` or `sync_size_from_len` for that.
  // Calling one of these functions is particularely useful if you get
  // a buf_t structure via parameters.
  //
  T2 len;

  //
  // size of the data in bytes
  // WARNING: this is not AUTOMICALLY synched with `len` :(
  // You have to call `sync_len_from_size` or `sync_size_from_len` for that.
  // Calling one of these functions is particularely useful if you get
  // a buf_t structure via parameters.
  //
  T2 size;

  /*
   * @brief
   *  Synchronize the `len` and `size` members.
   *  This functions considers `size` as the correct member and
   *  set `len` to its value. By default `len` will not be overwritten
   *  if its value is non zero. You can change that behaviour using the
   *  `force` parameter.
   *
   *  You usually want to use this function, when you get passed
   *  via parameters a buf_t structure and want to use the `len` member.
   *
   * @param force
   *  Whether to force the update of the `len` member. By default this
   *  parameter is set to `false`. Unless this parameter is set to `true`,
   *  the `len` member is only updated if it is equal to 0.
   *
   * @return
   *  updated length
   */
  inline T2 sync_len_from_size( bool force = false ) {
    if ( force || ! len ) len = size / sizeof( T1 );
    return len;
  }

  /*
   * @brief
   *  Synchronize the `len` and `size` members.
   *  This functions considers `len` as the correct member and
   *  set `len` to its value. By default `size` will not be overwritten
   *  if its value is non zero. You can change that behaviour using the
   *  `force` parameter.
   *
   *  You usually want to use this function, when you get passed
   *  via parameters a buf_t structure and want to use the `size` member.
   *
   * @param force
   *  Whether to force the update of the `size` member. By default this
   *  parameter is set to `false`. Unless this parameter is set to `true`,
   *  the `size` member is only updated if it is equal to 0.
   *
   * @return
   *  updated size
   */
  inline T2 sync_size_from_len( bool force = false ) {
    if ( force || ! size ) size = len * sizeof( T1 );
    return size;
  }
};

// in the context of shellcode you can only pass a single argument
// when creating a new thread for example
// so we wrap argument passing into a structure for portability
struct args_t {
  const char** arguments;
  uint32_t     length;
};

namespace imperium {
  //
  // NOTE: do not change the order of members of that struct
  // some features rely on this specific order
  // e.g. instance_t::clean, syscalls...
  //
  struct instance_t {
    //
    // context to find our instance in memory
    //
    uint32_t context;

    //
    // store already loaded functions
    //
    cached_module_t cached_modules;

    //
    // base address and size
    // of the implant
    //
    buf_t< void > base;

    void*    console;
    uint8_t* first_syscall;

    /*!
     * @brief
     *  get a pointer to the instance by reading the peb
     *  this one in located in the process heaps table
     *
     * @return
     *  pointer to the instance
     */
    static instance_t* find();

    /*!
     * @brief
     *  init the imperium instance
     *  you should init the instance and then start it.
     */
    static instance_t* init();

    /*
     * @brief
     *  your code entrypoint
     */
    void start( _In_ const args_t* args );

    /*
     * @brief
     *  free up buffers and clean instance related stuff
     *  this function automatically gets called when you quit
     */
    void clean();
  };
}  // namespace imperium

typedef PVOID( NTAPI* fnRtlAllocateHeap )( IN PVOID HeapHandle, IN OPTIONAL ULONG Flags, IN SIZE_T Size );

typedef PVOID(
    NTAPI* fnRtlReAllocateHeap )( IN PVOID HeapHandle, IN ULONG Flags, IN OPTIONAL PVOID BaseAddress, IN SIZE_T Size );

typedef PVOID( NTAPI* fnRtlFreeHeap )( IN PVOID HeapHandle, IN OPTIONAL ULONG Flags, IN OPTIONAL PVOID BaseAddress );

typedef BOOL( WINAPI* fnAttachConsole )( IN DWORD dwProcessId );

typedef HANDLE WINAPI( WINAPI* fnGetStdHandle )( IN DWORD nStdHandle );

typedef BOOL( WINAPI* fnWriteConsoleA )( IN HANDLE hConsoleOutput, IN const PVOID lpBuffer,
    IN DWORD nNumberOfCharsToWrite, OUT OPTIONAL LPDWORD lpNumberOfCharsWritten, LPVOID lpReserved );

typedef int( WINAPI* fnVsnprintf )( char* buffer, size_t count, const char* format, va_list argptr );

typedef int( WINAPI* fnSwprintf )( wchar_t* buffer, size_t count, const wchar_t* format, ... );

typedef PVOID( WINAPI* fnLoadLibraryA )( PCSTR Module );

typedef INT( WINAPI* fnMessageBoxA )( IN OPTIONAL HWND hWnd, IN OPTIONAL LPCSTR lpText, IN OPTIONAL LPCSTR lpCaption,
    IN UINT uType );

typedef HLOCAL( WINAPI* fnLocalFree )( _In_ HLOCAL hMem );

typedef NTSTATUS( WINAPI* fnNtClose )( _In_ HANDLE Handle );

#endif  // IMPERIUM_DEFS_H
