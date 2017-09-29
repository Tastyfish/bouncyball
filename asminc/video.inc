PPU_CTRL	:= $2000
PPU_MASK	:= $2001
PPU_STATUS	:= $2002

OAM_ADDR	:= $2003
OAM_DATA	:= $2004

PPU_SCROLL	:= $2005
PPU_ADDR	:= $2006
PPU_DATA	:= $2007

OAM_DMA		:= $4014

VBLANK_FLAG	:= $70

.struct Sprite
	YPos	.byte
	TileID	.byte
	Attrib	.byte
	XPos	.byte
.endstruct

.define SpriteAddress(no) ($0200 + no * .sizeof(Sprite))

PAL_BG0		= $01
PAL_BG1		= $05
PAL_BG2		= $09
PAL_BG3		= $0D
PAL_SPR0	= $11
PAL_SPR1	= $15
PAL_SPR2	= $19
PAL_SPR3	= $1D

PPUM_SPRITES			= %00010000
PPUM_EDGE_SPRITES		= %00000100
PPUM_BACKGROUNDS		= %00001000
PPUM_EDGE_BACKGROUNDS	= %00000010

.ifndef _VIDEO_EXPORT
	.importzp _vc_scrollx, _vc_scrolly, _vc_ctrl
	.import _vb_ClearOAM, _vb_FullCopyOAM, _vb_CopySprite, _vb_CopyOAM
	.import _vb_CopyPPU
	.import _v_SetBGColor, _v_SetPalette
	.import _v_WaitVBlank
	.import _v_AllocSprite, _v_FreeSprite
.endif
