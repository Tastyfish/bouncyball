#include <stdbool.h>

typedef unsigned char spriteID_t;
typedef unsigned char color_t;
typedef unsigned char tile_t;

#define COL_BLACK ((color_t)0x0F)

typedef struct {
	unsigned char y;
	tile_t tile;
	unsigned char attrib;
	unsigned char x;
} Sprite;

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
} pal_t;

extern void vb_ClearOAM();
extern void vb_FullCopyOAM();
extern void __fastcall__ vb_CopySprite(spriteID_t id);
extern void __fastcall__ vb_CopyOAM(unsigned char start, unsigned char length);
extern void vb_CopyPPU(void* start, int length);
extern void __fastcall__ v_SetBGColor(color_t color);
extern void __fastcall__ v_SetPalette(pal_t pal_id, color_t col_1, color_t col_2, color_t col_3);
extern void v_WaitVBlank();
extern void vb_DisableAll();
extern void __fastcall__ vb_EnableSprites(bool enable);
extern void __fastcall__ vb_EnableBackgrounds(bool enable);
extern void __fastcall__ v_ScrollBackground(unsigned char x, unsigned char y);
extern void __fastcall__ v_BigScrollBackground(int x, int y);

extern spriteID_t v_AllocSprite();
extern void __fastcall__ v_FreeSprite(spriteID_t sprite);

extern void __fastcall__ vb_DecompressNT(int ppuAddr, const char* ptr);
extern void __fastcall__ v_DecompressQLEChunk(int ppuAddr, char quarter, const char* ptr);
extern void __fastcall__ v_DecompressToRAM(void* dest, void* src);

extern void v_FadeIn(int delay, ...);
extern void v_FadeOut(int delay, ...);
extern bool v_FadeStep();
