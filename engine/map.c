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

typedef struct {
	char entityID;
	char ix;
	char iy;
} qre_entry_t;

const map_header_t* header;
bool map_orientation;
int map_refX, map_refY; // pixel pos we're in
int map_lx, map_ly; // actual viewport TL reference for sprites anc scrolling
int sx, sy; // section we're in
int sectionXCount, sectionYCount;
int map_width, map_height;

// what section is loaded in the given quarter
// 1st index is NT 0 or 1, 2nd is LTR quarter
int sectionLoaded[2][4] = {};

qre_entry_t qreQueue[32];
extern mapload_entity_t mapload_entities[];
extern size_t mapload_entity_count;
extern char sectionCols[2][4][16*16/8];

void assignSection(char nt, char q, int sectionID);
void updateVSections(void);
void updateHSections(void);
void scroll(void);

/***********************************************
** Load up the map around the position (map_refX,map_refY)
***********************************************/
void map_Load(const map_header_t* map) {
	memset(sectionLoaded, 0xFF, sizeof(sectionLoaded));

	header = map;
	sectionXCount = header->qrv->width;
	sectionYCount = header->qrv->height;
	map_width = sectionXCount * 16;
	map_height = sectionYCount % 2 ? sectionYCount * 15 + 1 : sectionYCount * 15;

	// wrap it to be valid
	map_refX = header->startx % (sectionXCount << 7);

	map_refY = (sectionYCount >> 1) * 240;
	if(sectionYCount % 2)
		map_refY += 128;
	map_refY = header->starty % map_refY;

	sx = map_refX / 128;
	sy = map_refY / 240;
	if(map_refY % 240 > 128) {
		sy = sy * 2 + 1;
	} else {
		sy = sy * 2;
	}
}

void map_SetOrientation(MapOrientation o) {
	map_orientation = o;

	if(map_orientation) {
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

int compareMapload(const void* a, const void* b) {
	return ((mapload_entity_t*)a)->entityID - ((mapload_entity_t*)b)->entityID;
}

mapload_entity_t mapload_key;

void assignSection(char nt, char q, int sectionID) {
	int* pLoaded = &sectionLoaded[nt][q];

	qre_entry_t* currentEntity = qreQueue;
	int eox = sx * 128;
	int eoy = sy % 2 ? sy * 120 + 8 : sy * 120;
	mapload_entity_t* mapload;

	if(*pLoaded != sectionID) {
		// visual
		v_DecompressQRVChunk(nt ? 0x2400 : 0x2000, q,
			((char*)header->qrv) + header->qrv->sectionOffsets[sectionID]);

		// entity
		bzero(qreQueue, sizeof(qreQueue));
		v_DecompressQREChunk(qreQueue, q,
			((char*)header->qre) + header->qre->sectionOffsets[sectionID]);

		while(currentEntity->entityID != 0) {
			mapload_key.entityID = currentEntity->entityID;
			mapload = bsearch(&mapload_key, mapload_entities,
				mapload_entity_count, sizeof(mapload_entity_t),
				&compareMapload);
			if(mapload) {
				e_Create(mapload->ctor, eox + currentEntity->ix * 8, eoy + currentEntity->iy * 8, currentEntity->entityID);
			}
			++currentEntity;
		}

		// col
		v_DecompressQRCChunk(&sectionCols[nt][q][0],
			((char*)header->qrc) + header->qrc->sectionOffsets[sectionID]);

		// mark loaded
		*pLoaded = sectionID;
	}
}

// Do actual raw scrolling for internal use only
void scroll(void) {
	if(map_orientation) {
		map_lx = map_refX - 127;
		map_ly = (sy & ~1) * 240;
	} else {
		map_lx = (sx & ~1) * 256;
		map_ly = map_refY - 119;
	}
	v_BigScrollBackground(512 + map_lx % 512, 480 + map_ly % 480);
}

void map_MoveTo(int rx, int ry) {
	int newsx, newsy;

	if(map_refX == rx && map_refY == ry)
		return;

	map_refX = rx;
	map_refY = ry;

	newsx = map_refX / 128;
	newsy = map_refY / 240;
	if(map_refY % 240 >= 128) {
		newsy = newsy * 2 + 1;
	} else {
		newsy *= 2;
	}

	if(map_orientation) {
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
