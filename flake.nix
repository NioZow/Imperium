{
  description = "Imperium cross-compilation environment";

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
          mingw.stdenv.cc
          mingw.buildPackages.gcc
          mingw.buildPackages.binutils
          nasm
          binutils
          gnumake
          python313
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
            echo "  make pic     - Build x64 shellcode"
            echo "  make dll     - Build x64 dll"
            echo "  make exe     - Build x64 exe"
            echo "  make loader  - Build x64 shellcode loader"
            echo "  make clean   - Clean binaries"
            echo ""
            
            echo "Environment ready!"
          '';
        };
      }
    );
}
