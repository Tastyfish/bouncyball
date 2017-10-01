#include <stdbool.h>

#ifndef _VIDEO_H
#define _VIDEO_H

typedef unsigned char spriteID_t;
typedef unsigned char color_t;
typedef unsigned char tile_t;

#define COL_BLACK ((color_t)0x0F)

typedef struct {
	unsigned char y;
	tile_t tile;
	unsigned char attrib;
	unsigned char x;
} sprite_t;

#define SpriteIDToAddress(id) ((Sprite*)(0x0500 + (id) * sizeof(Sprite)))
#define AddressToSpriteID(addr) ((spriteID_t)((((int)(addr)) - 0x0500) / sizeof(Sprite)))

typedef enum {
	PAL_BG0		= 0x01,
	PAL_BG1		= 0x05,
	PAL_BG2		= 0x09,
	PAL_BG3		= 0x0D,
	PAL_SPR0	= 0x11,
	PAL_SPR1	= 0x15,
	PAL_SPR2	= 0x19,
	PAL_SPR3	= 0x1D
} palid_t;

typedef color_t pal_t;
typedef color_t palset_t;

extern void vb_ClearOAM(void);
extern void vb_FullCopyOAM(void);
extern void __fastcall__ vb_CopySprite(spriteID_t id);
extern void __fastcall__ vb_CopyOAM(unsigned char start, unsigned char length);
extern void vb_CopyPPU(const void* start, int length);

extern void __fastcall__ v_SetBGColor(color_t color);
extern void __fastcall__ v_SetPalette(palid_t pal_id, color_t col_1, color_t col_2, color_t col_3);
extern void __fastcall__ v_CopyPalette(palid_t pal_id, const pal_t* source);
extern void __fastcall__ v_CopyBackgroundPalset(const palset_t* source);
extern void __fastcall__ v_CopySpritePalset(const palset_t* source);

extern void v_WaitVBlank(void);
extern void vb_DisableAll(void);

extern void __fastcall__ vb_EnableSprites(bool enable);
extern void __fastcall__ vb_EnableBackgrounds(bool enable);

extern void __fastcall__ v_ScrollBackground(unsigned char x, unsigned char y);
extern void __fastcall__ v_BigScrollBackground(int x, int y);

extern sprite_t* v_AllocSprite(void);
extern void __fastcall__ v_FreeSprite(sprite_t* sprite);

extern void __fastcall__ vb_DecompressNT(int ppuAddr, const void* src);
extern void __fastcall__ v_DecompressQLEChunk(int ppuAddr, char quarter, const void* src);
extern void __fastcall__ v_DecompressQRVChunk(int ppuAddr, char quarter, const void* src);
extern void __fastcall__ v_DecompressQRCChunk(void* dest, char quarter, const void* src);
extern void __fastcall__ v_DecompressQREChunk(void* dest, char quarter, const void* src);
extern void __fastcall__ v_DecompressToRAM(void* dest, const void* src);

extern void __fastcall__ v_FadeIn(int delay, const palset_t* to);
extern void v_FadeOut(int delay);
extern bool v_FadeStep(void);

#endif
