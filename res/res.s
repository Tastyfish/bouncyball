.macro incres symbol, file
	.local data
	.export symbol
data:
	.incbin file
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

incres _QLE_SPLASH, "water map.qle"
