#include <stdlib.h>
#include "game.h"

// Pick random number from lo to hi
unsigned int crand(unsigned int lo, unsigned int hi) {
	// Conceptually, returns lo + (rand() * (hi-lo) / MAX)
	// where MAX is the highest possible value
	// we can't really do this without overflowing on the multiplication

	// it should be evenly distributed across all bits (other than that MSB is 0 because rand() returns signed)
	return lo + rand() % (hi - lo + 1);
}
