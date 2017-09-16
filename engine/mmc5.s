;
; File generated by cc65 v 2.13.9
;
	.fopt		compiler,"cc65 v 2.13.9"
	.setcpu		"6502"
	.smart		on
	.autoimport	on
	.case		on
	.debuginfo	off
	.importzp	sp, sreg, regsave, regbank
	.importzp	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4
	.macpack	longbranch
	.import		_qsort
	.export		_vm_AddScanlineCallback
	.export		_vm_RemoveScanlineCallback
	.importzp	_mmc_sl_ptr
	.import		_mmc5_sl_counter
	.import		_mmc5_irq_ctrl
	.export		_scanlineCount
	.export		_scanline_callbacks
	.export		handle_hblank
	.export		_sorterFn
	.export		_resort

.segment	"DATA"

_scanlineCount:
	.word	$0000

.segment	"BSS"

_scanline_callbacks:
	.res	24,$00

; ---------------------------------------------------------------
; __near__ void* __near__ vm_AddScanlineCallback (unsigned char, __near__ __near__ function returning void*)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_vm_AddScanlineCallback: near

.segment	"CODE"

	jsr     decsp2
	lda     _scanlineCount+1
	bne     L0012
	lda     _scanlineCount
	cmp     #$08
	bne     L0012
	ldx     #$00
	txa
	jmp     incsp5
L0012:	lda     _scanlineCount
	ldx     _scanlineCount+1
	sta     regsave
	stx     regsave+1
	clc
	adc     #$01
	bcc     L001A
	inx
L001A:	sta     _scanlineCount
	stx     _scanlineCount+1
	lda     regsave
	ldx     regsave+1
	jsr     mulax3
	clc
	adc     #<(_scanline_callbacks)
	tay
	txa
	adc     #>(_scanline_callbacks)
	tax
	tya
	jsr     stax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$04
	lda     (sp),y
	ldy     #$00
	sta     (ptr1),y
	iny
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	sta     ptr1
	stx     ptr1+1
	ldy     #$03
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	dey
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
	lda     _scanlineCount+1
	bne     L001F
	lda     _scanlineCount
	cmp     #$01
	bne     L001F
	ldy     #$04
	lda     (sp),y
	sta     _mmc5_sl_counter
	ldx     #$00
	lda     #$80
	sta     _mmc5_irq_ctrl
	jmp     incsp5
L001F:	ldy     #$04
	lda     (sp),y
	jsr     pusha0
	lda     _mmc_sl_ptr
	sta     ptr1
	lda     _mmc_sl_ptr+1
	sta     ptr1+1
	lda     (ptr1,x)
	jsr     tosicmp0
	bcs     L0027
	lda     _mmc_sl_ptr
	ldx     _mmc_sl_ptr+1
	clc
	adc     #$03
	bcc     L002A
	inx
L002A:	sta     _mmc_sl_ptr
	stx     _mmc_sl_ptr+1
L0027:	jsr     _resort
	jmp     incsp5

.endproc

; ---------------------------------------------------------------
; void __near__ vm_RemoveScanlineCallback (__near__ void*)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_vm_RemoveScanlineCallback: near

.segment	"CODE"

	jsr     pushw0sp
	lda     _scanlineCount+1
	bne     L002E
	lda     _scanlineCount
	cmp     #$01
	bne     L002E
	lda     #$00
	sta     _mmc5_irq_ctrl
	jmp     L0033
L002E:	ldy     #$01
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	sta     ptr1
	stx     ptr1+1
	lda     #$FF
	ldy     #$00
	sta     (ptr1),y
	iny
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	jsr     pusha0
	lda     _mmc_sl_ptr
	sta     ptr1
	lda     _mmc_sl_ptr+1
	sta     ptr1+1
	lda     (ptr1,x)
	jsr     tosicmp0
	bcs     L0036
	lda     _mmc_sl_ptr
	ldx     _mmc_sl_ptr+1
	sec
	sbc     #$03
	bcs     L0039
	dex
L0039:	sta     _mmc_sl_ptr
	stx     _mmc_sl_ptr+1
L0036:	jsr     _resort
L0033:	lda     _scanlineCount
	sec
	sbc     #$01
	sta     _scanlineCount
	bcs     L003C
	dec     _scanlineCount+1
L003C:	jmp     incsp4

.endproc

; ---------------------------------------------------------------
; void __fastcall__ handle_hblank (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	handle_hblank

; Hand optimized
.segment	"CODE"
	lda     _scanlineCount
	ora     _scanlineCount+1
	beq     L0042
	ldy     #$02
	lda     (_mmc_sl_ptr),y
	sta     ptr1+1
	dey
	lda     (_mmc_sl_ptr),y
	sta		ptr1
	dey
	lda     (_mmc_sl_ptr),y
	jsr     pusha
	lda     ptr1
	ldx     ptr1+1
	jsr     callax
	inc     _mmc_sl_ptr+1
	bne     L0001
	inc     _mmc_sl_ptr+1
L0001:
	lda     #<(_scanline_callbacks+24)
	ldx     #>(_scanline_callbacks+24)
	cpx     _mmc_sl_ptr+1
	bne     L0042
	cmp     _mmc_sl_ptr
	bne     L0042
	lda     #<(_scanline_callbacks)
	sta     _mmc_sl_ptr
	lda     #>(_scanline_callbacks)
	sta     _mmc_sl_ptr+1
L0042:	rts

.endproc

; ---------------------------------------------------------------
; int __near__ sorterFn (__near__ const void*, __near__ const void*)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_sorterFn: near

.segment	"CODE"

	ldy     #$03
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	sta     ptr1
	stx     ptr1+1
	ldx     #$00
	lda     (ptr1,x)
	jsr     pusha0
	ldy     #$03
	lda     (sp),y
	tax
	dey
	lda     (sp),y
	sta     ptr1
	stx     ptr1+1
	ldx     #$00
	lda     (ptr1,x)
	jsr     tosulta0
	jmp     incsp4

.endproc

; ---------------------------------------------------------------
; void __near__ resort (void)
; ---------------------------------------------------------------

.segment	"CODE"

.proc	_resort: near

.segment	"CODE"

	jsr     decsp6
	lda     #<(_scanline_callbacks)
	ldy     #$04
	sta     (sp),y
	iny
	lda     #>(_scanline_callbacks)
	sta     (sp),y
	lda     _scanlineCount
	ldy     #$02
	sta     (sp),y
	iny
	lda     _scanlineCount+1
	sta     (sp),y
	tya
	ldy     #$00
	sta     (sp),y
	iny
	lda     #$00
	sta     (sp),y
	lda     #<(_sorterFn)
	ldx     #>(_sorterFn)
	jmp     _qsort

.endproc

