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
	int centerx, centery;

	for(; currentBox < CHECKED_COLBOX_END; ++currentBox) {
		if(currentBox->right >= refX && currentBox->bottom >= refY
			&& currentBox->x < refRight && currentBox->y < refBottom) {

			// solve the middle of the overlap
			centerx = MAX(currentBox->x, refX);
			centerx += MIN(currentBox->right, refRight);
			centerx /= 2;
			centery = MAX(currentBox->y, refY);
			centery += MIN(currentBox->bottom, refBottom);
			centery /= 2;

			collide = currentBox->onCollide;
			if(collide)
				collide(currentBox, ref, centerx - currentBox->x, centery - currentBox->y);
			else if(refCollide)
				refCollide(ref, currentBox, centerx - refX, centery - refY);
		}
	}
}
