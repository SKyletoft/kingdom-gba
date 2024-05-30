#include "hexes.h"

namespace hexes {

CubeCoord &CubeCoord::operator=(CubeCoord vec) {
	// We can't edit q, r, s, so we destroy this and create a new CubeCoord in
	// place
	this->~CubeCoord();
	return *(new (this) CubeCoord{vec});
}

} // namespace hexes
