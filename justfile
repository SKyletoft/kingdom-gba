setup-env:
	git clone https://github.com/devkitPro/libtonc.git
	git clone https://github.com/devkitPro/maxmod.git
	sed -i 's/\r//g' libtonc/include/*.h libtonc/src/*.c libtonc/src/tte/*.c maxmod/include/* maxmod/source/*

mgba:
	make
	mgba-qt srpg-engine.elf -4

clean:
	-rm -rf libtonc maxmod
	-doas rm -rf build build
