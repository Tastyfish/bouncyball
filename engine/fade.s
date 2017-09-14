.include "zeropage.inc"
.include "video.inc"

.import ppubuf_put
.import pusha, pushax, tosumulax, tosudivax, addysp, subysp

.export _v_FadeIn, _v_FadeOut, _v_FadeStep

.bss
delay:		.res 1
destvals:	.res 12
step:		.res 2
destStep:	.res 2
dir:		.res 1

.code

; Fade the palette in
; void v_FadeIn(int delay, color_t colors ...)
.proc _v_FadeIn
	lda #0
	beq FadeX
.endproc

; Fade the palette out
; void v_FadeOut(int delay, color_t colors ...)
.proc _v_FadeOut
	lda #1
	bne FadeX
.endproc

.proc FadeX
	sta dir

	; check correct length
	cpy #26
	beq doit
	brk

doit:
	; dump the entire param set into delay,destvals
	dey
	dey
	ldx #0
copyloop:
	lda (sp),y
	sta delay,x
	dey
	dey
	inx
	cpx #13
	bne copyloop

	; destStep = delay * 5 - 1
	clc
	lda delay
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

	ldy #26
	jsr addysp

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
	lda #0 ; offset for destvals
	sta ptr3
	lda #1 ; pal id
	sta ptr4
loop:
	lda ptr4
	jsr pusha

	.repeat 2, i
		ldy ptr3
		lda destvals,y
		and sreg
		bne .ident(.sprintf("notBlack%02d", i))
		lda #$0F
	.ident(.sprintf("notBlack%02d", i)):
		jsr pusha
		inc ptr3
	.endrepeat
	ldy ptr3
	lda destvals,y
	and sreg
	bne notBlackA
	lda #$0F
notBlackA:
	inc ptr3

	jsr _v_SetPalette
	.repeat 4
		inc ptr4
	.endrepeat
	lda ptr3
	cmp #12
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
