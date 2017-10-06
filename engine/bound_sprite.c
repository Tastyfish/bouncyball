#include <stddef.h>
#include "video.h"
#include "map.h"

#define NUM_BOUND_SPRITES 64
bound_sprite_t boundSprites[NUM_BOUND_SPRITES];
#define BOUNDSPRITES_END (boundSprites + NUM_BOUND_SPRITES)

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

bound_sprite_t* map_AllocBoundSprite(void) {
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
	int y = bs->y - 1;
	sprite_t* s = bs->sprite;

	if(map_orientation) {
		if(x < map_lx || x > map_lx + 255) {
			// offscreen
			s->y = 0xFF;
		} else {
			// place relative to camera
			s->x = x - map_lx;
			s->y = y - map_ly;
		}
	} else {
		if(y < map_ly || y > map_ly + 239) {
			// offscreen
			s->y = 0xFF;
		} else {
			// place relative to camera
			s->x = x - map_lx;
			s->y = y - map_ly;
		}
	}
}
