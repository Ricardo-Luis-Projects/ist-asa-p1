# Shell environment for development on NixOS
{ pkgs ? import <nixpkgs> {} }: with pkgs; mkShell {
  nativeBuildInputs = [
    pkg-config
    gcc
    valgrind
    clang-tools
    python39
    python39Packages.matplotlib
  ];
}
