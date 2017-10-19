.include "zeropage.inc"
.include "mem.inc"

.import asrax4

.export _umod15
.export _umod30
.export _umod60
.export _umod240

.segment	"LOWCODE"

; ---------------------------------------------------------------
; unsigned char __near__ umod15 (unsigned int)
; ---------------------------------------------------------------

.proc _umod15
	; from http://homepage.divms.uiowa.edu/~jones/bcd/mod.shtml

	stx tmp1
	clc
	adc tmp1
	bcc doa
	ldx #1
	bne doac
doa:
	ldx #0
doac:
	sta tmp2

	jsr asrax4
	sta tmp1
	lda tmp2
	and #$0F
	clc
	adc tmp1
	bcc dob
	inx
dob:

	cmp #15
	bcs check2
	rts

check2:
	cmp #30
	bcs check3
	sec
	sbc #15
	rts

check3:
	sec
	sbc #30
	rts
.endproc

; ---------------------------------------------------------------
; unsigned char __near__ umod30 (unsigned int)
; ---------------------------------------------------------------

.proc _umod30
	; derived from relationship wih x%30 vs x%15 and x%2 from factoring

	sta tmp3
	jsr _umod15
	sta tmp1 ; m15
	lda tmp3
	and #1
	sta tmp3 ; m4

	; (m15 & 1) == m2 ? m15 : m15 + 15
	lda tmp1
	and #1
	cmp tmp3
	bne high15

	lda tmp1
	rts

high15:
	lda tmp1
	clc
	adc #15
	rts
.endproc

; ---------------------------------------------------------------
; unsigned char __near__ umod60 (unsigned int)
; ---------------------------------------------------------------

.proc _umod60
	sta tmp3
	jsr _umod15
	sta tmp1 ; m15
	and #3
	sta tmp2 ; m15_3
	lda tmp3
	and #3
	sta tmp3 ; m4

	; if(m4 == m15_3) return m15;
	lda tmp3
	cmp tmp2
	bne check2
	lda tmp1
	rts

check2:
	; if((m4 = m4 + 1 & 3) == m15_3) return m15 + 15;
	inc tmp3
	lda tmp3
	and #3
	sta tmp3
	cmp tmp2
	bne check3
	lda tmp1
	clc
	adc #15
	rts

check3:
	; if((m4 = m4 + 1 & 3) == m15_3) return m15 + 30;
	inc tmp3
	lda tmp3
	and #3
	sta tmp3
	cmp tmp2
	bne check4
	lda tmp1
	clc
	adc #30
	rts

check4:
	; return m15 + 45;
	lda tmp1
	clc
	adc #45
	rts
.endproc

.proc _umod240
	sta tmp3
	jsr _umod15
	sta tmp1 ; m15
	and #15
	sta tmp2 ; m15_15
	lda tmp3
	and #15
	sta tmp3 ; m16

	; if(m16 == m15_15) return m15;
	lda tmp3
	cmp tmp2
	bne check2
	lda tmp1
	rts

check2:
	; if((m4 = m4 + 1 & 15) == m15_3) return m15 + 15;
	lda tmp1
	clc
	adc #15
	sta tmp1
	inc tmp3
	lda tmp3
	and #15
	sta tmp3
	cmp tmp2
	bne check2
	lda tmp1
	rts
.endproc