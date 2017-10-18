#include <string.h>
#include "math.h"
#include "collision.h"

#define NUM_CB (64)
collision_box_t checkedCollisionBoxes[NUM_CB];
#define CHECKED_COLBOX_END (checkedCollisionBoxes + NUM_CB)

collision_box_t uncheckedCollisionBoxes[NUM_CB];
#define UNCHECKED_COLBOX_END (uncheckedCollisionBoxes + NUM_CB)

collision_box_t* col_AllocBox(bool checked, int x, int y, int width, int height) {
	register collision_box_t* box = checked ? checkedCollisionBoxes : uncheckedCollisionBoxes;
	collision_box_t* const end = checked ? CHECKED_COLBOX_END : UNCHECKED_COLBOX_END;

	for(; box < end; ++box) {
		if(!box->right && !box->bottom) {
			box->x = x;
			box->y = y;
			box->right = x + width;
			box->bottom = y + height;
			return box;
		}
	}

	return NULL;
}

void col_FreeBox(collision_box_t* cb) {
	memset(cb, 0, sizeof(collision_box_t));
}

void col_Test(collision_box_t* ref) {
	register collision_box_t* currentBox = checkedCollisionBoxes;

	int refX = ref->x;
	int refY = ref->y;
	int refRight = ref->right;
	int refBottom = ref->bottom;
	collide_cb_t refCollide = ref->onCollide;

	collide_cb_t collide;
	int ix, iy, ir, ib;
	signed char ox, oy;

	for(; currentBox < CHECKED_COLBOX_END; ++currentBox) {
		if(currentBox->right >= refX && currentBox->bottom >= refY
			&& currentBox->x < refRight && currentBox->y < refBottom) {

			// solve overlap points
			ix = MAX(currentBox->x, refX);
			iy = MAX(currentBox->y, refY);
			ir = MIN(currentBox->right, refRight);
			ib = MIN(currentBox->bottom, refBottom);

			collide = currentBox->onCollide;
			if(collide) {
				if(ix == refX) {
					// currentBox is more left
					if(ir == refRight) {
						// currentBox is also more right (outside)
						ox = (char)(currentBox->right - currentBox->x) / 2;
					} else {
						// overall on the left
						ox = ix - currentBox->x;
					}
				} else if(ir == refRight) {
					// overall on the right
					ox = ir - currentBox->x;
				} else {
					// currentBox is inside
					ox = (char)(ir - ix) / 2;
				}

				if(iy == refY) {
					// currentBox is more left
					if(ib == refBottom) {
						// currentBox is also more right (outside)
						oy = (char)(currentBox->bottom - currentBox->y) / 2;
					} else {
						// overall on the left
						oy = iy - currentBox->y;
					}
				} else if(ib == refBottom) {
					// overall on the right
					oy = ib - currentBox->y;
				} else {
					// currentBox is inside
					oy = (char)(ib - iy) / 2;
				}

				collide(currentBox, ref, ox, oy);
			} else if(refCollide) {
				if(ix == refX) {
					// ref is more right
					if(ir == refRight) {
						// ref is also more left (inside)
						ox = (char)(ir - ix) / 2;
					} else {
						// overall on the right
						ox = ir - refX;
					}
				} else if(ir == refRight) {
					// overall on the left
					ox = ix - refX;
				} else {
					// currentBox is outside
					ox = (char)(currentBox->right - currentBox->x) / 2;
				}

				if(iy == refY) {
					// ref is more right
					if(ib == refBottom) {
						// ref is also more left (inside)
						oy = (char)(ib - iy) / 2;
					} else {
						// overall on the right
						oy = ib - refY;
					}
				} else if(ib == refBottom) {
					// overall on the left
					oy = iy - refY;
				} else {
					// currentBox is outside
					oy = (char)(currentBox->bottom - currentBox->y) / 2;
				}

				refCollide(ref, currentBox, ox, oy);
			}
		}
	}
}
