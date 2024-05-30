#pragma once

#include <array>
#include <cstddef>

extern "C" {
#include <tonc_types.h>
}

namespace input {

class Button {
  public:
	enum InternalButton {
		A = 0,
		B,
		Select,
		Start,
		Right,
		Left,
		Up,
		Down,
		R,
		L,
	};

  private:
	InternalButton button = A;

  public:
	bool operator==(InternalButton const &rhs) const;
	bool operator==(Button const &rhs) const;

	constexpr operator size_t() const { return (size_t)this->button; }

	Button(InternalButton const &from)
		: button(from) {}
	Button() {}
};

class InputState {
  public:
	enum InternalInputState { Up, Pressed, Held, Released };

  private:
	InternalInputState state = InputState::Up;

  public:
	bool is_down() const;
	bool is_up() const;

	bool operator==(InternalInputState const &rhs) const;
	bool operator==(InputState const &rhs) const;

	InputState(InternalInputState const &from)
		: state(from) {}
	InputState() {}
};

void poll();
std::array<InputState, 10> const &get_input();
InputState get_button(Button);
s8 horizontal_direction();
s8 vertical_direction();

} // namespace input
