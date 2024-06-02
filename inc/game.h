#pragma once

#include "state.h"

namespace game {

class Game : public state::Mode {
	s32 player_position = 0;
	bool player_facing_right = false;
	s32 camera_position = -120;

  public:
	void update() override;
	void vsync_hook() override;
	void restore() override;

	Game() {}
};

} // namespace game
