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

#define SpriteIDToAddress(id) ((Sprite*)(0x0200 + (id) * sizeof(Sprite)))
#define AddressToSpriteID(addr) ((spriteID_t)(((addr) - 0x0200) / sizeof(Sprite)))


#define PAL_BG0		(0x01)
#define PAL_BG1		(0x05)
#define PAL_BG2		(0x09)
#define PAL_BG3		(0x0D)
#define PAL_SPR0	(0x11)
#define PAL_SPR1	(0x15)
#define PAL_SPR2	(0x19)
#define PAL_SPR3	(0x1D)

extern void v_ClearOAM();
extern void v_FullCopyOAM();
extern void __fastcall__ v_CopySprite(spriteID_t id);
extern void __fastcall__ v_CopyOAM(unsigned char start, unsigned char length);
extern void v_CopyPPU(void* start, int length);
extern void __fastcall__ v_SetBGColor(color_t color);
extern void __fastcall__ v_SetPalette(palID_t pal_id, color_t col_1, color_t col_2, color_t col_3);
extern void v_WaitVBlank();
extern void v_DisableAll();
extern void __fastcall__ v_EnableSprites(bool enable);
extern void __fastcall__ v_EnableBackgrounds(bool enable);

extern spriteID_t v_AllocSprite();
extern void __fastcall__ v_FreeSprite(spriteID_t sprite);
