#include "math.h"

#pragma code-name ("LOWCODE")

// Vxy is the velocity vector. Both input and output.
// Nxy is the normal of the face being hit. Assumed normalized
void reflectd(decimal_t *vx, decimal_t *vy, decimal_t nx, decimal_t ny) {
	// -2(v dot n)
	int n2vdotn = (*vx * nx + *vy * ny) / -8;

	// outputs back to vx & vy
	*vx += n2vdotn * nx / 16;
	*vy += n2vdotn * ny / 16;
}
