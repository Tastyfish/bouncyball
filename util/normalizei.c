#include "math.h"

void normalizei(int *x, int *y) {
	int mag = sqrti(*x * *x + *y * *y);
	*x /= mag;
	*y /= mag;
}
