.include "helpers.inc"

.export rt_Init, memset
.export ppha, pphax, ppla, pplax, asp
.exportzp sp, addr1, addr2, sreg

.zeropage

sp:		.addr 0
addr1:	.addr 0
addr2:	.addr 0
sreg:	.byte 0

.code

.proc rt_Init
	lda #$00
	sta sp
	lda #$80
	sta sp+1
	rts
.endproc

.proc ppha
	dec16x sp
	ldy #0
	sta (sp),y
	rts
.endproc

.proc ppla
	ldy #0
	lda (sp),y
	inc16 sp
	rts
.endproc

.proc pphax
	pha
	sec
	lda sp
	sbc #2
	sta sp
	bcs done
	dec sp+1
	
done:
	pla
	
	ldy #1
	sta (sp),y
	dey
	txa
	sta (sp),y
	
	rts
.endproc

.proc pplax
	
	ldy #0
	lda (sp),y
	tax
	iny
	lda (sp),y
	
	.repeat 2
		inc16 sp
	.endrepeat
	
	rts
.endproc

.proc asp
	clc
	adc sp
	sta sp
	lda #0
	adc sp+1
	sta sp+1
	rts
.endproc

; Set memory region to value
; __fastcall__ void memset(void* start, int length, byte value)
.proc memset
length	= 1
start	= 3
; param mangled
end		= length
	sta sreg
	
	; quickly change length into an end value (end = length + start + 1)
	clc
	ldy #start
	lda (sp),y
	ldy #length
	adc (sp),y
	adc #1
	sta (sp),y
	ldy #start-1
	lda (sp),y
	ldy #length-1
	adc (sp),y
	sta (sp),y
	
	; The core memset loop
	
	; use addr1 as the copy ptr, A for value
	ldy #start
	lda (sp),y
	sta addr1
	dey
	lda (sp),y
	sta addr1+1
loop:
	ldy #0
	lda sreg
	sta (addr1),y
	inc16 addr1
	
	lda addr1,y
	ldy #end
	cmp (sp),y
	bne loop
	
	ldy #0
	lda addr1+1,y
	ldy #end-1
	cmp (sp),y
	bne loop
	
	lda #4
	jmp asp
.endproc
