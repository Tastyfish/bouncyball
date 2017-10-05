#include <collision.h>
#include <map.h>

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
	int ox = 0, oy = 0;
	collide_cb_t collide = box->onCollide;

	if(map_TestCol(x, y)) {
		ox = x % 8 - 8;
		oy = y % 8 - 8;
	}
	if(map_TestCol(r, y)) {
		ox += r % 8;
		oy += y % 8 - 8;
	}
	if(map_TestCol(x, b)) {
		ox += x % 8 - 8;
		oy += b % 8;
	}
	if(map_TestCol(r, b)) {
		ox += r % 8;
		oy += b % 8;
	}

	if((ox || oy) && collide)
		collide(box, ox, oy);
}

bool map_TestCol(int x, int y) {
	char q = (x / 16 % 2) | (y % 30 < 16) * 2;
	char ix = x % 16 / 8;
	char iy = q >= 2 ? y % 30 - 16 : y % 30;
	char* addr;

	if(map_orientation) {
		// horizontal
		x %= 64;
		y %= 30;
		addr = &sectionCols[x / 32 % 2][q][ix + iy * ROW_SPAN];
	} else {
		// vertical
		x %= 32;
		y %= 60;
		addr = &sectionCols[y / 30 % 2][q][ix + iy * ROW_SPAN];
	}

	return *addr & (1 << (x % 8));
}
