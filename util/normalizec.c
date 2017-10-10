#include "math.h"

#pragma code-name ("LOWCODE")

void normalizec(signed char *x, signed char *y) {
	char mag = sqrtc(*x * *x + *y * *y);
	*x /= mag;
	*y /= mag;
}
