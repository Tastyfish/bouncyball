#include <stdlib.h>
#include "math.h"

#pragma code-name ("LOWCODE")

// Like normalizei, but optimized for the normal being in a cardinal direction
// Returns true if X flipped
bool reflectcard(int *vx, int *vy, int nx, int ny) {
	if(!nx && !ny)
		return false;

	if(abs(nx) >= abs(ny)) {
		*vx = nx < 0 ? abs(*vx) : -abs(*vx);
		return true;
	} else {
		*vy = ny < 0 ? abs(*vy) : -abs(*vy);
		return false;
	}
}
