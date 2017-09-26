#include "math.h"

void normalizec(signed char *x, signed char *y) {
	char mag = sqrtc(*x * *x + *y * *y);
	*x /= mag;
	*y /= mag;
}
