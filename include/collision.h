#include "video.h"

typedef void (*collide_cb_t)(struct collision_s* box, unsigned int ox, unsigned int oy);

typedef struct collision_s {
	int x, y;
	int right, bottom;
	collide_cb_t onCollide;
} collision_box_t;

extern collision_box_t* col_AllocBox(bool checked, int x, int y, int width, int height);
extern void col_FreeBox(collision_box_t* cb);

extern void col_Test(collision_box_t* ref);