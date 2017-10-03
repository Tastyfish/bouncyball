.macro incres symbol, file
	.local data
	.export symbol
data:
	.incbin file
symbol:
	.addr data
.endmacro

.macro incmap symbol, filename, sx, sy
	.local data, qrv, qrc, qre
	.export symbol
data:
	.word sx, sy
	.addr qrv, qrc, qre
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

.segment "RODATA"

incres _PAL_BALLS, "balls.pal"
incres _PAL_WATERSHIP, "watership.pal"

incres _NAM_BG, "bg.rle"
incres _NAM_JOKE, "joke.rle"
incres _NAM_DIRECTIONS, "directions.rle"

incmap _MAP_TITLE, "title", 128, 0
incmap _MAP_PINBALL1, "pinball1", 27*8, 29*8

.export _S_MUSIC
_S_MUSIC:
	.addr theme_music_data
.include "theme.s"

.export _S_SFX
_S_SFX:
	.addr sounds
.include "sfx.s"
