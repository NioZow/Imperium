{
  description = "Stardust cross-compilation environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
       
        mingw = pkgs.pkgsCross.mingwW64;
        
        buildInputs = with pkgs; [
          # Use unwrapped clang to avoid target conflicts
          llvmPackages.clang-unwrapped
          llvmPackages.bintools-unwrapped
          
          # (pkgs.writeShellScriptBin "x86_64-w64-mingw32-gcc" ''
          #   exec ${pkgs.pkgsCross.mingwW64.buildPackages.gcc}/bin/x86_64-w64-mingw32-gcc "$@"
          # '')
          # (pkgs.writeShellScriptBin "x86_64-w64-mingw32-g++" ''
          #   exec ${pkgs.pkgsCross.mingwW64.buildPackages.gcc}/bin/x86_64-w64-mingw32-g++ "$@"
          # '')
          
          # Native tools
          gtest
          nasm
          binutils
          gnumake
          uv
        ];
        
      in
      {
        devShells.default = pkgs.mkShell {
          inherit buildInputs;
          shellHook = ''
            echo "Imperium cross-compilation environment"
            echo "======================================"
            echo ""
            echo "Available commands:"
            echo "  make shellcode  - Build x64 shellcode x64"
            echo "  make clean      - Clean binaries"
            echo ""
            
            # set up PATH to use unwrapped clang
            export PATH="${pkgs.llvmPackages.clang-unwrapped}/bin:$PATH"

            export MINGW_PREFIX="${pkgs.lib.getDev mingw.stdenv.cc.cc}"
            export CLANG_RESOURCE_DIR="${pkgs.llvmPackages.clang-unwrapped.lib}/lib/clang/19"
            
            echo "Environment ready!"
          '';
        };
      }
    );
}
