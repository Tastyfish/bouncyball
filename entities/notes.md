# Entity Cheatsheet

##The standard arguments for a maploaded entity are:
1. `int x`
2. `int y`
3. `char (sprite id - 0x80)` that represented it in the map
	* Useful such that an entity can have multiple versions,
	such as flipped versions, or different graphics.

##The way to allocate failable resources in constructors safely.
1. Set the destructor before loading anything.
2. Try to load. If the result is `NULL`, destroy the entity and return. Yes, it's safe and recommended to call e_Destroy(this) in a constructor to abort.
3. Set a parameter immediately after allocating the resource, so it's already set before trying the next one.
3. In the destructor, check if resource params are non-null before freeing them.

###e.g.
```c
sprite_t* ls, * rs;

this->onDestroy = DestroyExample;

ls = v_AllocSprite();
if(!ls) {
	e_Destroy(this);
	return;
}
param_lsprite = ls;

rs = v_AllocSprite();
if(!rs) {
	e_Destroy(this);
	return;
}
param_rsprite = rs;

...in DestroyExample()...

sprite_t* ls = param_lsprite;
sprite_t* rs = param_rsprite;

if(ls) {
	ls->y = 0xFF;
	v_FreeSprite(ls);
}
if(rs) {
	rs->y = 0xFF;
	v_FreeSprite(rs);
}
```

##Referencing params without magic numbers throughout
Create a `#define` containing the deference and indexing of the param. Remember when picking index numbers that all param types overlap in a union.
###e.g.
```c
#define param_lsprite	(this->paramp[0])
#define param_rsprite	(this->paramp[1])
```

##What to allocate
Assuming the entity will be used in-game, remember to use `bound_sprite_t`s from map.h.
If the sprite should always be relative to the screen, then a `sprite_t` will do.
Remember that basic `sprite_t`s should have their `y` set to `0xF0`--`0xFF` to be made invisible before freeing.