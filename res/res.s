.segment "CHARS"

.incbin "a.chr"

.segment "RODATA"

.export _NAM_BG
_NAM_BG:
.incbin "bg.rle"

.export _NAM_JOKE
_NAM_JOKE:
.incbin "joke.rle"

.export _NAM_DIRECTIONS
_NAM_DIRECTIONS:
.incbin "directions.rle"
