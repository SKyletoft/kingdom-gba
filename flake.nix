{
	description = "A very basic flake";

	inputs = {
		nixpkgs.url     = "github:nixos/nixpkgs/nixpkgs-unstable";
		flake-utils.url = "github:numtide/flake-utils";
	};

	outputs = { self, nixpkgs, flake-utils }:
		flake-utils.lib.eachDefaultSystem(system:
			let
				pkgs = import nixpkgs {
					inherit system;
					config = {
						permittedInsecurePackages = ["freeimage-unstable-2021-11-01"];
						allowUnfree = true;
					};
				};
				gba-tools = pkgs.stdenv.mkDerivation {
					pname = "gba-tools";
					version = "1.2.0";

					src = pkgs.fetchFromGitHub {
						owner = "devkitPro";
						repo = "gba-tools";
						rev = "v1.2.0";
						sha256 = "sha256-8I0RbrLVUy/+klEQnsPvXWQYx5I49QGjqqw33RLzkOY=";
					};

					nativeBuildInputs = with pkgs; [ stdenv automake autoconf ];

					buildPhase = "./autogen.sh && ./configure && make gbafix";

					installPhase = ''
						mkdir -p $out/bin
						cp gbafix $out/bin
					'';
				};
				grit = pkgs.stdenv.mkDerivation {
					pname = "grit";
					version = "0.8.17";

					src = pkgs.fetchFromGitHub {
						owner = "devkitPro";
						repo = "grit";
						rev = "v0.8.17";
						sha256 = "sha256-dIF59akcVaRH5/y7ZQ2FlGIOD8q3RS4bt1k5cEVqtUw=";
					};

					nativeBuildInputs = with pkgs; [
						stdenv
						automake
						autoconf
						libtool
						intltool
					];
					buildInputs = with pkgs; [ freeimage ];

					buildPhase = "./autogen.sh && ./configure && make grit";

					installPhase = ''
						mkdir -p $out/bin
						cp grit $out/bin
					'';
				};
			in {
				packages = { inherit gba-tools grit; };
				devShells.default = pkgs.mkShell {
					nativeBuildInputs = with pkgs; [
						rustup
						rust-analyzer

						mgba
						gba-tools
						grit
						gcc-arm-embedded-13

						aseprite

						gdb
					];
				};
			}
		);
}
