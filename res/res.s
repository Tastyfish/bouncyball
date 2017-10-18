.macro incres symbol, file
	.local data
	.export symbol
data:
	.incbin file
symbol:
	.addr data
.endmacro

.macro incmap symbol, filename, sx, sy, xdata
	.local data, qrv, qrc, qre
	.export symbol
data:
	.word sx, sy
	.addr qrv, qrc, qre, xdata
qrv:
	.incbin .sprintf("%s.qrv", filename)
qrc:
	.incbin .sprintf("%s.qrc", filename)
qre:
	.incbin .sprintf("%s.qre", filename)
symbol:
	.addr data
.endmacro

.segment "CHARS"

.incbin "game.chr"
.include "neschar.inc"
.incbin "title.chr"

; Complete the char bank
.res $1800, $CC

.segment "RODATA"

incres _PAL_BALLS, "balls.pal"
incres _PAL_WATERSHIP, "watership.pal"

incres _NAM_BG, "bg.rle"
incres _NAM_JOKE, "joke.rle"
incres _NAM_DIRECTIONS, "directions.rle"

incmap _MAP_TITLE, "title", 128, 0, 0

; Mostly for infos
.scope mapx_pinball
start:
	.addr str0, str1, str2
str0:
	.byte "Use the D-Pad to move the ball around.",0
str1:
	.byte "Coins give you points.",0
str2:
	.byte "Sometimes there are multiple paths, but it may be harder to use one than the other.", 0
.endscope

incmap _MAP_PINBALL1, "pinball1", 27*8, 29*8, mapx_pinball::start


.export _S_MUSIC
_S_MUSIC:
	.addr theme_music_data
.include "theme.s"

.export _S_SFX
_S_SFX:
	.addr sounds
.include "sfx.s"
