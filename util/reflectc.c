#include "math.h"

#pragma code-name ("LOWCODE")

// Vxy is the velocity vector. Both input and output.
// Nxy is the normal of the face being hit. Assumed normalized
void reflectc(signed char *vx, signed char *vy, signed char nx, signed char ny) {
	// -2(v dot n)
	signed char n2vdotn = -2 * (*vx * nx + *vy * ny);

	// outputs back to vx & vy
	*vx += n2vdotn * nx;
	*vy += n2vdotn * ny;
}
