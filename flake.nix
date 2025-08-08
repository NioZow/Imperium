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
          compiledb
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
          name = "imperium";

          shellHook = ''
            export name="imperium"
          '';
        };
      }
    );
}
