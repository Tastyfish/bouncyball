#include "math.h"

// Vxy is the velocity vector. Both input and output.
// Nxy is the normal of the face being hit. Assumed normalized
void reflecti(int *vx, int *vy, int nx, int ny) {
	// -2(v dot n)
	int n2vdotn = -2 * (*vx * nx + *vy * ny);

	// outputs back to vx & vy
	*vx += n2vdotn * nx;
	*vy += n2vdotn * ny;
}
