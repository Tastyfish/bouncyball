#include <stdbool.h>
#include <stdlib.h>

#include "game.h"
#include "video.h"
#include "entity.h"
#include "entities.h"

extern void setup_splash();

void main() {
	setup_splash();
	vb_EnableSprites(true);
	vb_EnableBackgrounds(true);

	g_Run();
}
