with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "clion";
  # nativeBuildInputs = [ cmake gcc ];
  buildInputs = [ cmake gcc jetbrains.clion valgrind gdb ];
}