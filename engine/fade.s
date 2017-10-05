.include "zeropage.inc"
.include "video.inc"
.include "mem.inc"

.import ppubuf_put
.import tosumulax, tosudivax

.export _v_FadeIn, _v_FadeOut, _v_FadeStep

.bss
delay:		.res 1
destvals:	.res 16
step:		.res 2
destStep:	.res 2
dir:		.res 1

.segment "LOWCODE"

; Fade the palette in
; void v_FadeIn(int delay, const palset_t* to)
.proc _v_FadeIn
	; copy to destvals
	sta ptr1
	stx ptr1+1
	lda #<destvals
	sta ptr2
	lda #>destvals
	sta ptr2+1

	ldy #$00
loop:
	lda (ptr1),y
	sta (ptr2),y
	iny
	cpy #$10
	bne loop

	lda #0
	beq FadeX
.endproc

; Fade the palette out
; void v_FadeOut(int delay)
.proc _v_FadeOut
	lda #1
	bne FadeX
.endproc

.proc FadeX
	sta dir

	jsr popax
	sta delay

	; destStep = delay * 5 - 1
	clc
	adc delay
	adc delay
	adc delay
	adc delay
	sta destStep
	dec destStep
	lda destStep
	cmp #$FF
	bne setStep
	dec destStep+1

setStep:
	lda dir
	bne downDir
	lda #0 ; at beginning
	sta step
	sta step+1
	beq doneStep
downDir:
	lda destStep
	sta step
	lda destStep+1
	sta step+1
	dec step
	lda step
	cmp #$FF
	bne doneStep
	dec step+1
doneStep:

	; do a first step
	jmp _v_FadeStep
.endproc

; Step the fade one frame, returns true if completed
; bool v_FadeStep()
.proc _v_FadeStep
	; a 0 delay marks that there is no current fade
	lda delay
	bne doit
	lda delay+1
	bne doit

	; "done" because there is no fade happening
	lda #1
	rts

doit:
	; true step is step / delay
	lda step
	ldx step+1
	jsr pushax
	lda delay
	ldx #0
	jsr tosudivax
	; and the step is turned into mask of upper nibble of fade, 0th being true black
	lda sreg
	bne actualColor
	; it's black
	lda #00
	sta sreg
	beq preloop
actualColor:
	dec sreg
	lda sreg
	asl a
	asl a
	asl a
	asl a
	ora #$0F
	sta sreg

preloop:
	; loop all palettes
	lda #0 ; offset for destval and pal position
	sta tmp1
loop:
	ldx tmp1
	lda destvals,x
	and sreg
	cmp #$00
	bne notBlack
	lda #$0F ; make it black and not medium grey
notBlack:
	ldy #$3F
	jsr ppubuf_put

	inc tmp1
	lda tmp1
	cmp #$10
	bne loop

	lda dir
	bne dirDown

	inc step
	bne upDone
	inc step+1
upDone:
	; was last step? if so, mark done
	lda step
	cmp destStep
	bne done
	lda step+1
	cmp destStep+1
	bne done
	jmp finished

dirDown:
	dec step
	lda step
	cmp #$FF
	bne downDone
	dec step
downDone:
	; was last step? if so, mark done
	lda step
	bne done
	lda step+1
	bne done

finished:
	lda #0
	sta delay
	sta delay+1
	lda #1
	rts

done:
	lda #0
	rts
.endproc
