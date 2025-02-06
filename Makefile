out/game.gba: target/thumbv4t-none-eabi/release/rust-on-gba.gba
	mkdir -p out
	cp $< $@

target/thumbv4t-none-eabi/release/rust-on-gba.gba: target/thumbv4t-none-eabi/release/rust-on-gba.elf
	arm-none-eabi-objcopy -O binary $< $@
	gbafix $@

target/thumbv4t-none-eabi/release/rust-on-gba.elf: target/thumbv4t-none-eabi/release/rust-on-gba
	cp $< $@

target/thumbv4t-none-eabi/release/rust-on-gba:
	cargo build --release

mgba: target/thumbv4t-none-eabi/release/rust-on-gba
	mgba-qt $<

clean:
	cargo clean
	-rm out -rf

install: out/game.gba
	cp out/game.gba /run/media/$$USER/PHONE\ CARD

.PHONY: target/thumbv4t-none-eabi/release/rust-on-gba clean install mgba
