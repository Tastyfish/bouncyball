.segment "CHARS"

.incbin "a.chr"

.segment "RODATA"

.export _NAM_BG
_NAM_BG:
.incbin "bg.rle"
