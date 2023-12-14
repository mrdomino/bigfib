{ pkgs, stdenv }:
stdenv.mkDerivation {
  buildInputs = [ pkgs.gmp ];
  name = "bigfib";
  version = "0.2.0";
}
