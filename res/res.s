.macro incres symbol, file
	.local data
	.export symbol
data:
	.incbin file
symbol:
	.addr data
.endmacro

.macro incmap symbol, filename
	.local data, qrv, qrc, qre
	.export symbol
data:
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

.incbin "a.chr"
.include "neschar.inc"

.segment "RODATA"

incres _NAM_BG, "bg.rle"
incres _NAM_JOKE, "joke.rle"
incres _NAM_DIRECTIONS, "directions.rle"

incres _QLE_SPLASH, "water_map.qle"

incres _PAL_BALLS, "balls.pal"
incres _PAL_WATERSHIP, "watership.pal"

.export _S_MUSIC
_S_MUSIC:
	.addr theme_music_data
.include "theme.s"

.export _S_SFX
_S_SFX:
	.addr sounds
.include "sfx.s"
