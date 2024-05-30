file srpg-engine.elf
target extended-remote localhost:2345
load
b main
b _exit
layout split

define hook-step
	p *(volatile short int)0x04000006
end
define hook-next
	p *(volatile short int)0x04000006
end
