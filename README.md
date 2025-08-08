# Imperium

![Demo](assets/demo.png)

## Features

- Global instance stored in the `PEB`
- Native support for `indirect syscall`
- No `IAT`, can call any `win32` or `NT` API without having to worry about resolving it (through fancy syntax)
- Compile time string encryption using `consteval`
- Compile time string hashing using `consteval`
- Memory leak checker script
- Cool utils functions
- Macros to have nice errors printing
- Output to `shellcode`, `dll`, `exe`

## Template

```cpp
#include <imperium.h>

using namespace imperium;

/*
 * @brief
 *  common entry point to dll/exe/shellcode output
 *  so you just have to modify this function to have your code executed
 *  dll/exe/pic have their own true entrypoint which just call this function
 *
 * @param args
 *  user passed arguments
 *
 *  WARNING: this is optional and may be `nullptr`
 *
 *  NOTE:
 * - By default, the dll entrypoint passes its first argument.
 * - By default, the pic entrypoint passes its first argument.
 * - By default, the exe entrypoint passes wrapped argc and argv.
 *
 *  WARNING: for shellcode, arguments are get through the pointer that you can pass using `NtCreateThread` as argument
 *  the user calling `NtCreateThread` is responsible for ensuring the right format of the arguments.
 *  E.g. the first argument should be the program name (or dummy if not used) to ensure compability with exe format.
 *  If the passed pointer points to memory you are not allowed to access and you redeference it, the program will
 *  probably crash.
 */
declfn void instance_t::start( _In_opt_ const args_t* args ) {
  ENC_STRING( user32, "user32.dll" );
  ENC_STRING( hack, "HackThePlanet" );

  //
  // call LoadLibraryA to have the needed module in our peb
  // wont be able to resolve their functions otherwise
  //
  if ( ! win32::call< fnLoadLibraryA >( H_FUNC( "kernel32!LoadLibraryA" ), user32 ) ) {
    return;
  }

  win32_t::resolve( H_FUNC( "user32!MessageBoxA" ) )
      .exec< fnMessageBoxA >( nullptr, hack, args && args->length > 0 ? args->arguments[ 0 ] : "Imperium", MB_OK );
}
```

## Compiling

The recommended compilations methods are through `nix` and `docker` (docker just wraps `nix`).

> [!TIP]
> You can probably compile the project from any type of operating system if you meet the dependencies. However, your package manager is probably gonna use the latest version of the `g++` compiler, and it is known that when upgrading things break.
>
> So if you want to be sure that nothing breaks, I recommended using `nix` which uses a fixed version of the compiler.

### Nix

Just type `nix develop` and you will have all dependencies :

```sh
$ nix develop
$ make exe
-> compiling x64 exe
```

### Docker

You can build the image :

```sh
docker build -t imperium .
```

And the build the project :

```sh
docker run --rm -it -v .:/workspace imperium
Imperium cross-compilation environment
======================================

Available commands:
  make pic       - Build x64 shellcode
  make dll       - Build x64 dll
  make exe       - Build x64 exe
  make ldr       - Build x64 shellcode loader
  make clean     - Clean binaries
  compiledb make - Generate LSP definitions

Environment ready!
bash-5.2# make exe
-> compiling x64 exe
```

## Scripts

### Def

Takes an `url` as argument and returns the definition for the function ready to be copy and paste as a type :

```sh
$ ./scripts/def.py https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox
typedef int (WINAPI*fnMessageBox)(
  _In_opt_ HWND    hWnd,
  _In_opt_ LPCTSTR lpText,
  _In_opt_ LPCTSTR lpCaption,
  _In_           UINT    uType
)
```

### Memory leak checker

Checks your program for memory leaks. However, it only works when memory is allocated and freed using internal functions `mem::alloc` and `mem::free`. You also have to compile the binary using the `memleak` target.

```sh
python3 .\scripts\memleak_checker.py -e .\bin\imperium.x64.exe
Memory leak : src/main.cc::start::46
```

This runs the binary and check it for memory leaks.

# Credits

This project could not have been accomplished without the amazing work of [5pider](https://x.com/C5pider) on those projects :

- [Stardust](https://github.com/Cracked5pider/Stardust): I would have been able to make this template `PIC` otherwise
- Firebeam template: my template is highly inspired from it
