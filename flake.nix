{
	inputs = {
		nixpkgs.url     = "github:nixos/nixpkgs/nixpkgs-unstable";
		flake-utils.url = "github:numtide/flake-utils";
		devkitnix.url   = "github:SKyletoft/devkitnix";
	};

	outputs = { self, nixpkgs, flake-utils, devkitnix }:
		flake-utils.lib.eachDefaultSystem(system:
			let
				pkgs = nixpkgs.legacyPackages.${system};
				lib = nixpkgs.lib;
				devkit = devkitnix.packages.${system};
			in rec {
				devShells.default = pkgs.mkShell {
					nativeBuildInputs = with pkgs; [
						clang-tools_17
						gdb
						mgba
						just

						devkit.devkitARM

						mangohud
					];
					shellHook = ''
						export DEVKITPRO=${devkit.devkitARM}
						export DEVKITARM=${devkit.devkitARM}/devkitARM
					'';
				};
			}
		);
}
