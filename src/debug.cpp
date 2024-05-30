#include "debug.h"
#include "tty.h"

#include <cstdarg>

namespace debug {

tty::TtyMode TTY_MODE{};

void clear() { TTY_MODE.clear(); }
void print(const char *c) { TTY_MODE.print(c); }
void print(s32 i) { TTY_MODE.print(i); }
void printf(const char *format, ...) {
	va_list args;
	va_start(args, format);
	TTY_MODE.printf(format, args);
	va_end(args);
}
void print(const char *c, const size_t s) { TTY_MODE.print(c, s); }
void println() { TTY_MODE.println(); }
void println(s32 i) { TTY_MODE.println(i); }
void println(const char *c) { TTY_MODE.println(c); }
void println(const char *c, const size_t s) { TTY_MODE.println(c, s); }

} // namespace debug
