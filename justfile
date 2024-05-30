build: setup
	mkdir -p out
	docker run -v $(pwd):/srpg-engine builder

setup:
	docker build . -t builder

bash: setup
	docker run -v $(pwd):/srpg-engine -it builder /bin/bash

setup-env:
	git clone https://github.com/devkitPro/libtonc.git
	git clone https://github.com/devkitPro/maxmod.git
	sed -i 's/\r//g' libtonc/include/*.h libtonc/src/*.c libtonc/src/tte/*.c maxmod/include/* maxmod/source/*

mgba: build
	mgba-qt srpg-engine.elf -4

clean:
	-rm -rf libtonc maxmod
	-doas rm -rf build build
