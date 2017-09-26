.import _sectionLoaded, pushax, _memset

.segment "INIT"
.constructor setupMap
setupMap:
	lda #<_sectionLoaded
	ldx #>_sectionLoaded
	jsr pushax
	lda #$FF
	ldx #$00
	jsr pushax
	lda #<(2*4*2)
	ldx #>(2*4*2)
	jmp _memset
