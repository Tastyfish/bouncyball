#include "math.h"

#pragma code-name ("LOWCODE")

int sqrti(int x) {
	// digit-by-digit method

	int result = 0;
	int mask = 0x4000;

	while(mask > x)
		mask >>= 2;

	while(mask != 0) {
		if(x >= result + mask) {
			x -= result + mask;
			result = (result >> 1) + mask;
		} else {
			result >>= 1;
		}
		mask >>= 2;
	}

	return result;
}
