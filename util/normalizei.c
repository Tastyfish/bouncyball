#include "math.h"

#pragma code-name ("LOWCODE")

void normalizei(int *x, int *y) {
	int mag = sqrti(*x * *x + *y * *y);
	*x /= mag;
	*y /= mag;
}
