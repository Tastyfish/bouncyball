#include "math.h"

// Vxy is the velocity vector. Both input and output.
// Nxy is the normal of the face being hit. Assumed normalized
void reflectd(decimal_t *vx, decimal_t *vy, decimal_t nx, decimal_t ny) {
	// -2(v dot n)
	int n2vdotn = -2 * (*vx * nx + *vy * ny);

	// outputs back to vx & vy
	*vx += n2vdotn * nx / 16;
	*vy += n2vdotn * ny / 16;
}
