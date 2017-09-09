typedef unsigned char spriteID_t;
typedef unsigned char color_t;
typedef unsigned char palID_t;

#define COL_BLACK ((color_t)0x0F)

typedef struct {
	unsigned char YPos;
	unsigned char TileID;
	unsigned char Attrib;
	unsigned char XPos;
} Sprite;

#define SpriteAddress(id) ((Sprite*)(0x0200 + id * sizeof(Sprite)))


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
