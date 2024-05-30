#include "input.h"

#include <array>

extern "C" {
#include <tonc_memmap.h>
}

namespace input {

bool InputState::is_down() const {
	return this->state == InputState::Pressed
		   || this->state == InputState::Held;
}

bool InputState::is_up() const { return !this->is_down(); }

bool InputState::operator==(InternalInputState const &rhs) const {
	return this->state == rhs;
}

bool InputState::operator==(InputState const &rhs) const {
	return this->state == rhs.state;
}

std::array<InputState, 10> BUTTON_STATES;

void poll() {
	Button const buttons[] = {
		Button::A,
		Button::B,
		Button::L,
		Button::R,
		Button::Start,
		Button::Select,
		Button::Up,
		Button::Down,
		Button::Left,
		Button::Right
	};

	u16 raw = REG_KEYINPUT;

	for (auto button : buttons) {
		bool down = !(bool)(raw & (1 << button));
		if (down) {
			BUTTON_STATES[button] = BUTTON_STATES[button].is_down()
										? InputState::Held
										: InputState::Pressed;
		} else {
			BUTTON_STATES[button] = BUTTON_STATES[button].is_down()
										? InputState::Released
										: InputState::Up;
		}
	}
}

std::array<InputState, 10> const &get_input() { return BUTTON_STATES; }

InputState get_button(Button button) { return BUTTON_STATES[button]; }

s8 horizontal_direction() {
	return (u8)BUTTON_STATES[Button::Right].is_down()
		   - (u8)BUTTON_STATES[Button::Left].is_down();
}
s8 vertical_direction() {
	return (u8)BUTTON_STATES[Button::Down].is_down()
		   - (u8)BUTTON_STATES[Button::Up].is_down();
}

} // namespace input
