#include <stdbool.h>
#include "video.h"
#include "collision.h"
#include "entity.h"

#ifndef _MAP_H
#define _MAP_H

typedef char maptile_t;
typedef char mapattrib_t;

typedef enum {
	MO_VERTICAL = 0,
	MO_HORIZONTAL = 1
} MapOrientation;

typedef struct {
	int startx;
	int starty;
	struct qrx_header_s* qrv;
	struct qrx_header_s* qrc;
	struct qrx_header_s* qre;
} map_header_t;

typedef struct {
	sprite_t* sprite;
	int x;
	int y;
} bound_sprite_t;

typedef struct {
	unsigned char entityID;
	entity_ctor_t ctor;
} mapload_entity_t;

#define MAP_OLDTILE ((mapattrib_t)0xFF);

extern void map_Load(const map_header_t* map);
extern void map_SetOrientation(MapOrientation orientation);
extern void map_MoveTo(int rx, int ry);

extern bound_sprite_t* map_BindSprite(sprite_t* s);
extern void map_UnbindSprite(bound_sprite_t* bs);
extern bound_sprite_t* map_AllocBoundSprite();
extern void map_FreeBoundSprite(bound_sprite_t* bs);
extern void map_UpdateSprite(bound_sprite_t* bs);

extern bool map_TestCol(int x, int y);
extern void map_TestColBox(collision_box_t* box);

extern void __fastcall__ map_SetTile(int x, int y, maptile_t tile);

extern bool map_orientation;
extern int map_refX, map_refY;
extern int map_lx, map_ly;
extern int map_width, map_height;

#endif
