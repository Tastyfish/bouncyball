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
} map_header_t;

map_header_t* header;
bool orientation;
int refX, refY, ntX, ntY;
int lx, ly; // references for sprites

// what section is loaded in the given quarter
// 1st index is NT 0 or 1, 2nd is LTR quarter
extern int sectionLoaded[2][4] = {};

#define NUM_BOUND_SPRITES 64
bound_sprite_t boundSprites[NUM_BOUND_SPRITES];
bound_sprite_t* boundSprites_end = boundSprites + NUM_BOUND_SPRITES;

void assignSection(char nt, char q, int sectionID);
void updateVSections(void);
void updateHSections(void);

/***********************************************
** Load up the map around the position (refX,refY)
***********************************************/
void map_Load(const char* base, int rx, int ry) {
	header = (map_header_t*)base;

	// wrap it to be valid
	refX = rx % (header->width << 7);

	refY = (header->height >> 1) * 240;
	if(header->height % 2)
		refY += 128;
	refY = ry % refY;

	ntX = refX / 256;
	ntY = refY / 240;
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

void updateHSections(void) {
	int sx = refX >> 7, sy = refY >> 7;

	int xmax = MIN(sx + 1, header->width - 1);
	int ymax = MIN(sy | 1, header->height - 1);
	int x, y, yoffs;

	for(y = MAX(sy & ~1, 0); y <= ymax; ++y) {
		yoffs = y * header->width;
		for(x = MAX(sx - 1, 0); x <= xmax; ++x) {
			assignSection((x & 2) == 2, (x & 1) | ((y & 1) << 1), x + yoffs);
		}
	}
}

void updateVSections(void) {
	//int sx = refX >> 7, sy = refY >> 7;
	//int x, y;
}

void assignSection(char nt, char q, int sectionID) {
	int* pLoaded = &sectionLoaded[nt][q];

	if(*pLoaded != sectionID) {
		v_DecompressQLEChunk(nt ? 0x2400 : 0x2000, q,
			((char*)header) + header->sectionOffsets[sectionID]);

		*pLoaded = sectionID;
	}
}

void map_MoveTo(int rx, int ry) {
	int rntx, rnty;

	if(refX == rx && refY == ry)
		return;

	rntx = rx / 256;
	rnty = ry / 240;

	if(orientation) {
		// horizontal
		lx = rx - 128;
		ly = rnty * 240;
		v_BigScrollBackground(lx, ly);

		// check if we jumped row! Completely redo everything in this case.
		if(rnty != ntY) {
			refX = rx;
			refY = ry;
			ntX = rntx;
			ntY = rnty;
			map_SetOrientation(MO_HORIZONTAL);
		}
		// check if changed section
		else if(rntx != ntX) {
			refX = rx;
			refY = ry;
			ntX = rntx;
			// ntY is already correct due to if
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

bound_sprite_t* map_BindSprite(sprite_t* s) {
	// find an empty slot
	bound_sprite_t* bs = boundSprites;

	for(; bs < boundSprites_end; ++bs) {
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
