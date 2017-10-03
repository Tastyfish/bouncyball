#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "video.h"
#include "mmc5.h"
#include "map.h"

typedef struct qrx_header_s {
	int width;
	int height;
	int sectionOffsets[1];
} qrx_header_t;

const map_header_t* header;
bool orientation;
int refX, refY; // pixel pos we're in
int lx, ly; // actual viewport TL reference for sprites anc scrolling
int sx, sy; // section we're in
int sectionXCount, sectionYCount;

// what section is loaded in the given quarter
// 1st index is NT 0 or 1, 2nd is LTR quarter
extern int sectionLoaded[2][4] = {};

#define NUM_BOUND_SPRITES 64
bound_sprite_t boundSprites[NUM_BOUND_SPRITES];
#define BOUNDSPRITES_END (boundSprites + NUM_BOUND_SPRITES)

void assignSection(char nt, char q, int sectionID);
void updateVSections(void);
void updateHSections(void);
void scroll(void);

/***********************************************
** Load up the map around the position (refX,refY)
***********************************************/
void map_Load(const map_header_t* map) {
	memset(sectionLoaded, 0xFF, sizeof(sectionLoaded));

	header = map;
	sectionXCount = header->qrv->width;
	sectionYCount = header->qrv->height;

	// wrap it to be valid
	refX = header->startx % (sectionXCount << 7);

	refY = (sectionYCount >> 1) * 240;
	if(sectionYCount % 2)
		refY += 128;
	refY = header->starty % refY;

	sx = refX / 128;
	sy = refY / 240;
	if(refY % 240 > 128) {
		sy = sy * 2 + 1;
	} else {
		sy = sy * 2;
	}
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

	scroll();
}

void updateHSections(void) {
	int xmax = MIN(sx + 1, sectionXCount - 1);
	int ymax = MIN(sy | 1, sectionYCount - 1);
	int x, y, yoffs;

	for(y = MAX(sy & ~1, 0); y <= ymax; ++y) {
		yoffs = y * sectionXCount;
		for(x = MAX(sx - 1, 0); x <= xmax; ++x) {
			assignSection((x & 2) == 2, (x & 1) | ((y & 1) << 1), x + yoffs);
		}
	}
}

void updateVSections(void) {
	int xmax = MIN(sx | 1, sectionXCount - 1);
	int ymax = MIN(sy + 1, sectionYCount - 1);
	int x, y, yoffs;

	for(y = MAX(sy - 1, 0); y <= ymax; ++y) {
		yoffs = y * sectionXCount;
		for(x = MAX(sx & ~1, 0); x <= xmax; ++x) {
			assignSection((y & 2) == 2, (x & 1) | ((y & 1) << 1), x + yoffs);
		}
	}
}

void assignSection(char nt, char q, int sectionID) {
	int* pLoaded = &sectionLoaded[nt][q];

	if(*pLoaded != sectionID) {
		v_DecompressQRVChunk(nt ? 0x2400 : 0x2000, q,
			((char*)header->qrv) + header->qrv->sectionOffsets[sectionID]);

		*pLoaded = sectionID;
	}
}

// Do actual raw scrolling for internal use only
void scroll(void) {
	if(orientation) {
		lx = refX - 127;
		ly = (sy & ~1) * 240;
	} else {
		lx = (sx & ~1) * 256;
		ly = refY - 119;
	}
	v_BigScrollBackground(lx, ly);
}

void map_MoveTo(int rx, int ry) {
	int newsx, newsy;

	if(refX == rx && refY == ry)
		return;

	refX = rx;
	refY = ry;

	newsx = refX / 128;
	newsy = refY / 240;
	if(refY % 240 >= 128) {
		newsy = newsy * 2 + 1;
	} else {
		newsy *= 2;
	}

	if(orientation) {
		// horizontal
		// check if we jumped row! Completely redo everything in this case.
		if(newsy != sy) {
			sx = newsx;
			sy = newsy;
			map_SetOrientation(MO_HORIZONTAL);
		}
		// check if changed section
		else if(newsx != sx) {
			sx = newsx;
			// sy is already correct due to if
			updateHSections();
		}
		// otherwise still in same section, so nothing else to do but scroll
	} else {
		// vertical
		// check if we jumped row! Completely redo everything in this case.
		if(newsx != sx) {
			sx = newsx;
			sy = newsy;
			map_SetOrientation(MO_VERTICAL);
		}
		// check if changed section
		else if(newsy != sy) {
			sy = newsy;
			// sx is already correct due to if
			updateVSections();
		}
		// otherwise still in same section, so nothing else to do but scroll
	}

	scroll();
}

bound_sprite_t* map_BindSprite(sprite_t* s) {
	// find an empty slot
	bound_sprite_t* bs = boundSprites;

	for(; bs < BOUNDSPRITES_END; ++bs) {
		if(bs->sprite == 0) {
			bs->sprite = s;
			return bs;
		}
	}

	return NULL;
}

void map_UnbindSprite(bound_sprite_t* bs) {
	bs->sprite = NULL;
}

bound_sprite_t* map_AllocBoundSprite() {
	return map_BindSprite(v_AllocSprite());
}

void map_FreeBoundSprite(bound_sprite_t* bs) {
	bs->sprite->y = 0xFF;
	v_FreeSprite(bs->sprite);
	map_UnbindSprite(bs);
}

void map_UpdateSprite(bound_sprite_t* bs) {
	// Currently gross, should be fixed in future
	int x = bs->x;
	int y = bs->y;
	sprite_t* s = bs->sprite;

	if(orientation) {
		if(x < lx || x > lx + 255) {
			// offscreen
			s->y = 0xFF;
		} else {
			// place relative to camera
			s->x = x - lx;
			s->y = y - ly;
		}
	}
}


