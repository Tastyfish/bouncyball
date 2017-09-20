#include <stdbool.h>
#include <string.h>
#include "game.h"
#include "video.h"
#include "mmc5.h"
#include "map.h"

typedef struct {
	int width;
	int height;
	int sectionOffsets[1];
} MapHeader;

MapHeader* header;
bool orientation;
int refX, refY;

// what section is loaded in the given quarter
// 1st index is NT 0 or 1, 2nd is LTR quarter
extern int sectionLoaded[2][4] = {};

void assignSection(char nt, char q, int sectionID);
void updateVSections();
void updateHSections();

/***********************************************
** Load up the map around the position (refX,refY)
***********************************************/
void map_Load(const char* base, int rx, int ry) {
	header = (MapHeader*)base;

	// wrap it to be valid
	refX = rx % (header->width << 7);

	refY = (header->height >> 1) * 240;
	if(header->height % 2)
		refY += 128;
	refY = ry % refY;
}

void map_SetOrientation(MapOrientation o) {
	orientation = o;

	if(orientation) {
		// horizontal
		updateHSections();
	} else {
		// vertical
		updateVSections();
	}
}

void updateHSections() {
	int sx = refX >> 7, sy = refY >> 7;

	int xmax = MIN(sx + 1, header->width - 1);
	int ymax = MIN(sy | 1, header->height - 1);
	int x, y;

	for(y = MAX(sy & ~1, 0); y <= ymax; y++) {
		for(x = MAX(sx - 1, 0); x <= xmax; x++) {
			assignSection((x & 2) == 2, (x & 1) | ((y & 1) << 1), x + y * header->width);
		}
	}
}

void updateVSections() {
	//int sx = refX >> 7, sy = refY >> 7;
	//int x, y;
}

void tickup() {
	__asm__("cli\nnop\nsei");
}

void assignSection(char nt, char q, int sectionID) {
	int* pLoaded = &sectionLoaded[nt][q];

	if(*pLoaded != sectionID) {
		//v_WaitVBlank();
		//vb_DisableAll();
		vb_DecompressQLEChunk(nt ? 0x2400 : 0x2000, q,
			((char*)header) + header->sectionOffsets[sectionID]);
		//vb_EnableBackgrounds(true);
		//vb_EnableSprites(true);
		*pLoaded = sectionID;
	}
}

void map_MoveTo(int rx, int ry) {
	if(refX == rx && refY == ry)
		return;

	if(orientation) {
		// horizontal
		v_BigScrollBackground(rx - 128, (ry / 240) * 240);

		// check if we jumped row! Completely redo everything in this case.
		if(ry / 240 != refY / 240) {
			refX = rx;
			refY = ry;
			map_SetOrientation(MO_HORIZONTAL);
			return;
		}
		// check if changed section
		else if(rx / 240 != refY / 240) {
			refX = rx;
			refY = ry;
			updateHSections();
		}
		// just in same section, barely anything to do
		else {
			refX = rx;
			refY = ry;
		}
	} else {
		// vertical

	}
}
