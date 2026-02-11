{
  description = "An L-system exploration device";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [ "x86_64-linux" ];#"aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forEachSupportedSystem = f: nixpkgs.lib.genAttrs supportedSystems (system: f {
        pkgs = import nixpkgs { inherit system; };
      });
    in
    {
      packages = forEachSupportedSystem ( { pkgs }: rec {
          aristid = pkgs.stdenv.mkDerivation {
            pname = "aristid";
            version = "0.1.0";

            src = ./.;

            nativeBuildInputs = with pkgs; [
              raylib
            ];
            buildInputs = with pkgs; [
              raylib
            ];

            buildPhase = ''
              make aristid
            '';

            installPhase = ''
              mkdir -p $out/bin
              cp ./build/aristid $out/bin/
            '';

            meta = with pkgs.lib; {
              description = "An L-system exploration device";
              license = licenses.mit;
              maintainers = [];
              platforms = platforms.unix;
            };
         };
         default = aristid;
      });

      devShells = forEachSupportedSystem ({ pkgs }: {
        default = pkgs.mkShell.override
          {
            # Override stdenv in order to change compiler:
            # stdenv = pkgs.clangStdenv;
          }
          {
            nativeBuildInputs = with pkgs; [
              raylib
            ];
            buildInputs = with pkgs; [
              raylib
            ];
            packages = with pkgs; [
              raylib
              packcc
              clang-tools
            ] ++ (if stdenv.hostPlatform.system == "aarch64-darwin" then [ ] else [ gdb ]);
          };
      });
    };
}
