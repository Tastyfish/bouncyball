#include "video.h"

#ifndef _MAP_H
#define _MAP_H

typedef enum {
	MO_VERTICAL = 0,
	MO_HORIZONTAL = 1
} MapOrientation;

typedef struct {
	sprite_t* sprite;
	int x;
	int y;
} bound_sprite_t;

void map_Load(const char* base, int rx, int ry);
void map_SetOrientation(MapOrientation orientation);
void map_MoveTo(int rx, int ry);

bound_sprite_t* map_BindSprite(sprite_t* s);
void map_UnbindSprite(bound_sprite_t* bs);
bound_sprite_t* map_AllocBoundSprite();
void map_FreeBoundSprite(bound_sprite_t* bs);
void map_UpdateSprite(bound_sprite_t* bs);

#endif
