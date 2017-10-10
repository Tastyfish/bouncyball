#include "math.h"

#pragma code-name ("LOWCODE")

void normalized(decimal_t *x, decimal_t *y) {
	int mag = sqrti(*x * *x + *y * *y);
	*x *= 16;
	*x /= mag;
	*y *= 16;
	*y /= mag;
}
