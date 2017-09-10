#include <stdlib.h>
#include "game.h"

// Pick random number from lo to hi
unsigned int crand(unsigned int lo, unsigned int hi) {
	// Conceptually, returns lo + (rand() * (hi-lo) / MAX)
	// where MAX is the highest possible value
	// we can't really do this without overflowing on the multiplication

	// so, let's take the inverse of the fraction at the end, which will actually be > 1
	unsigned int fraction = 0x7FFF / (hi - lo);

	// and then divide the random value by it to get the correct result (although with rounding down error)

	return CLAMP(lo + rand() / fraction, lo, hi);
}
