#![no_std]
#![no_main]

use core::fmt::Write;

use bytemuck::TransparentWrapper;
use gba::prelude::*;

#[panic_handler]
fn panic_handler(info: &core::panic::PanicInfo) -> ! {
	#[cfg(debug_assertions)]
	if let Ok(mut logger) = MgbaBufferedLogger::try_new(MgbaMessageLevel::Fatal) {
		writeln!(logger, "{info}").ok();
	}
	loop {}
}

// #[allow(dead_code)]
// const FOO: Align4<[u8; 14]> = include_aligned_bytes!("foo.txt");

unsafe extern "C" {
	#[link_name = "king2Tiles"]
	safe static KING_2_TILES: [[u32; 8]; 128 / 8];
	#[link_name = "king2Pal"]
	safe static KING_2_PAL: [u16; 256];
}

#[link_section = ".ewram"]
static FRAME_KEYS: GbaCell<KeyInput> = GbaCell::new(KeyInput::new());

#[link_section = ".iwram"]
extern "C" fn irq_handler(_: IrqBits) {
	// We'll read the keys during vblank and store it for later.
	FRAME_KEYS.write(KEYINPUT.read());
}

#[no_mangle]
extern "C" fn main() -> ! {
	RUST_IRQ_HANDLER.write(Some(irq_handler));
	DISPSTAT.write(DisplayStatus::new().with_irq_vblank(true));
	IE.write(IrqBits::VBLANK);
	IME.write(true);

	restore();

	loop {
		render();
		VBlankIntrWait();
	}
}

fn render() {
	let mut sprite = ObjAttr::new();
	sprite.set_y(70);
	sprite.set_x(0);
	sprite.set_tile_id(0);
	sprite.set_style(ObjDisplayStyle::Normal);
	// sprite.0.with_shape(ObjShape::)
	sprite.1 = sprite.1.with_size(2); // 32x32

	OBJ_ATTR_ALL.index(0).write(sprite);
}

fn restore() {
	// Display settings

	const BACKGROUND: BackgroundControl = BackgroundControl::new()
		.with_bpp8(false)
		.with_charblock(3)
		.with_screenblock(0)
		.with_size(0) // Should be 32?!
		.with_priority(3);
	const DISPLAY: DisplayControl = DisplayControl::new()
		.with_video_mode(VideoMode::_0)
		.with_show_bg0(true)
		.with_show_obj(true)
		.with_obj_vram_1d(true);

	BG0CNT.write(BACKGROUND);
	DISPCNT.write(DISPLAY);

	// Load player sprite

	let tiles: &[Tile4; 16] = &KING_2_TILES;
	let palette: &[Color] = Color::wrap_slice(&KING_2_PAL);

	OBJ_TILES
		.as_region()
		.sub_slice(0..tiles.len())
		.write_from_slice(tiles);
	OBJ_PALETTE
		.as_region()
		.sub_slice(0..palette.len())
		.write_from_slice(palette);
}
