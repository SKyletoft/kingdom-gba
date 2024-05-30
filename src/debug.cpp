#include "debug.h"
#include "tty.h"
#include <cstdarg>

namespace debug {

tty::TtyMode tty_mode{};

void clear() { tty_mode.clear(); }
void print(const char *c) { tty_mode.print(c); }
void print(s32 i) { tty_mode.print(i); }
void printf(const char *format, ...) {
	va_list args;
	va_start(args, format);
	tty_mode.printf(format, args);
	va_end(args);
}
void print(const char *c, const size_t s) { tty_mode.print(c, s); }
void println() { tty_mode.println(); }
void println(s32 i) { tty_mode.println(i); }
void println(const char *c) { tty_mode.println(c); }
void println(const char *c, const size_t s) { tty_mode.println(c, s); }

} // namespace debug
