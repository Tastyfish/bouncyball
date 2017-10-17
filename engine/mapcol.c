#include <stddef.h>
#include "collision.h"
#include "map.h"

// width of row in bytes
#define ROW_SPAN (16/8)

// Collision data as [nt][1 or 0 in reading order]
extern char sectionCols[2][4][ROW_SPAN*16];
char sectionCols[2][4][ROW_SPAN*16];

void map_TestColBox(collision_box_t* box) {
	int x = box->x;
	int y = box->y;
	int r = box->right - 1;
	int b = box->bottom - 1;
	collide_cb_t collide = box->onCollide;
	char oflags = 0;
	signed char ox, oy;

	if(!collide)
		return;

	if(map_TestCol(x, y)) {
		oflags |= 0x05; // left up
		//map_SetTile(x/8, y/8, 0x08);
	}
	if(map_TestCol(r, y)) {
		oflags |= 0x06; // right up
		//map_SetTile(r/8, y/8, 0x08);
	}
	if(map_TestCol(x, b)) {
		oflags |= 0x09; // left down
		//map_SetTile(x/8, b/8, 0x08);
	}
	if(map_TestCol(r, b)) {
		oflags |= 0x0A; // right down
		//map_SetTile(r/8, b/8, 0x08);
	}

	if((oflags & 0x03) == 0x01) {
		// left
		ox = 8 - (char)x % 8;
	} else if((oflags & 0x03) == 0x02) {
		// right
		ox = (char)(r - x) - (char)r % 8;
	} else {
		ox = (char)(r + 1 - x) / 2;
	}

	if((oflags & 0x0C) == 0x04) {
		// up
		oy = 8 - (char)y % 8;
	} else if((oflags & 0x0C) == 0x08) {
		// down
		oy = (char)(b - y) - (char)b % 8;
	} else {
		oy = (char)(b + 1 - y) / 2;
	}

	if(oflags)
		collide(box, NULL, ox, oy);
}

bool map_TestCol(int x, int y) {
	char iy = y % 240;
	char q = ((char)(x / 128) % 2) | (iy < 128 ? 0 : 2);
	char ix = (char)(x % 128) / 8;
	char nt = map_orientation ? (char)(x / 256) % 2 : (char)(y / 240) % 2;
	iy = q >= 2 ? (char)(iy - 128) / 8 : iy / 8;

	return sectionCols[nt][q][ix / 8 + iy * ROW_SPAN] & (1 << (ix % 8));
}
