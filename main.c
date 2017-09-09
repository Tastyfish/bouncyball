#include <stdbool.h>
#include "video.h"

extern void g_Init();

void main() {
	int i;

	g_Init();
	v_EnableSprites(true);

	while(true) {
		for(i = 0; i < 255; i++) {
			v_WaitVBlank();

			v_SetBGColor(i);
		}
	}
}
