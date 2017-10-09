.include "zeropage.inc"
.include "video.inc"
.include "mem.inc"

.import ppubuf_put
.import tosumulax, tosudivax

.export _v_FadeIn, _v_FadeOut, _v_FadeStep

.bss
delay:		.res 1
destvals:	.res 32
step:		.res 2
destStep:	.res 2
dir:		.res 1

.segment "LOWCODE"

; Fade the palette in
; void v_FadeIn(int delay, const palset_t* bg, const palset_t* spr)
.proc _v_FadeIn
	; copy spr to destvals+$10
	sta ptr1
	stx ptr1+1
	lda #<(destvals+$10)
	sta ptr2
	lda #>(destvals+$10)
	sta ptr2+1

	ldy #$00
bgloop:
	lda (ptr1),y
	sta (ptr2),y
	iny
	cpy #$10
	bne bgloop

	; copy bg to destvals
	jsr popax
	sta ptr1
	stx ptr1+1
	lda #<destvals
	sta ptr2
	lda #>destvals
	sta ptr2+1

	ldy #0
sprloop:
	lda (ptr1),y
	sta (ptr2),y
	iny
	cpy #$10
	bne sprloop

	lda #0
	beq FadeX
.endproc

; Fade the palette out
; void v_FadeOut(int delay)
.proc _v_FadeOut
	jsr pushax
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
	; loop all palette values
	lda #0 ; offset for destval and pal position
	sta tmp1
loop:
	ldx tmp1
	; don't write bg values beyond first
	beq @setupWrite
	txa
	and #<3 ; checking if (x % 4) == 0
	beq @noWrite
@setupWrite:
	lda destvals,x
	and sreg
	cmp #$00
	bne @notBlack
	lda #$0F ; make it black and not medium grey
@notBlack:
	ldy #$3F
	jsr ppubuf_put

@noWrite:
	inc tmp1
	lda tmp1
	cmp #$20
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
