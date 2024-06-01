#pragma once

#include <cstddef>

extern "C" {
#include <tonc_types.h>
}

namespace state {

extern size_t CURRENT_STATE;
extern size_t NEXT_STATE;
extern size_t LAST_STATE;
extern bool BLACKED_OUT;

/// The mode baseclass is an abstraction to handle switching between game
/// states.
///
class Mode {
  private:
  public:
	virtual bool blackout();
	virtual void update() = 0;
	virtual void always_update() {}
	virtual void suspend() {}
	virtual void restore() {}
	virtual void vsync_hook() {}

	Mode() {}
	virtual ~Mode() {}
};

} // namespace state
