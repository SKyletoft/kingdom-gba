use std::process::Command;

fn main() -> anyhow::Result<()> {
	Command::new("mkdir").args(["-p", "target/gfx"]).output()?;

	println!("pwd: {}", std::env::var("PWD")?);
	Command::new("grit")
		.args(["gfx/king2.png", "-gt", "-gB4", "-R!", "-otarget/gfx/king2"])
		.output()?;
	cc::Build::new()
		.compiler("arm-none-eabi-gcc")
		.file("target/gfx/king2.s")
		.compile("graphics");
	println!("cargo::rustc-link-lib=graphics");

	Ok(())
}
