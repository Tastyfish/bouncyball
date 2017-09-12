#include <stdbool.h>

typedef unsigned char spriteID_t;
typedef unsigned char color_t;
typedef unsigned char palID_t;

#define COL_BLACK ((color_t)0x0F)

typedef struct {
	unsigned char y;
	unsigned char tileID;
	unsigned char attrib;
	unsigned char x;
} Sprite;

#define SpriteIDToAddress(id) ((Sprite*)(0x0500 + (id) * sizeof(Sprite)))
#define AddressToSpriteID(addr) ((spriteID_t)(((addr) - 0x0500) / sizeof(Sprite)))


#define PAL_BG0		(0x01)
#define PAL_BG1		(0x05)
#define PAL_BG2		(0x09)
#define PAL_BG3		(0x0D)
#define PAL_SPR0	(0x11)
#define PAL_SPR1	(0x15)
#define PAL_SPR2	(0x19)
#define PAL_SPR3	(0x1D)

extern void vb_ClearOAM();
extern void vb_FullCopyOAM();
extern void __fastcall__ vb_CopySprite(spriteID_t id);
extern void __fastcall__ vb_CopyOAM(unsigned char start, unsigned char length);
extern void vb_CopyPPU(void* start, int length);
extern void __fastcall__ v_SetBGColor(color_t color);
extern void __fastcall__ v_SetPalette(palID_t pal_id, color_t col_1, color_t col_2, color_t col_3);
extern void v_WaitVBlank();
extern void vb_DisableAll();
extern void __fastcall__ vb_EnableSprites(bool enable);
extern void __fastcall__ vb_EnableBackgrounds(bool enable);
extern void __fastcall__ v_ScrollBackground(unsigned char x, unsigned char y);
extern void __fastcall__ v_BigScrollBackground(int x, int y);
extern void vb_FlushScroll();

extern spriteID_t v_AllocSprite();
extern void __fastcall__ v_FreeSprite(spriteID_t sprite);

extern void __fastcall__ vb_DecompressNT(int ppuAddr, void* ptr);
