.include "zeropage.inc"

.macpack	longbranch
.import		_scanline_callbacks, _scanlineCount
.import		callax, pusha

.export		handle_hblank, _vm_SetNametableMirroring, _vbm_SetNametableMirroring

.exportzp _vc_sl_ptr        := $80 ; 16 bit
.exportzp _vc_nt_mapping    := $82
.exportzp _vc_sl_i          := $83
.exportzp _vc_chr_lbank     := $84
.exportzp _vc_chr_ubank     := $85
.exportzp _vc_bg_bank       := $86
; Remember to update famitone zp start

.export _mmc5_chr_mode      := $5101
.export _mmc5_nt_mapping    := $5105
.export _mmc5_chr_lbank     := $5123
.export _mmc5_chr_ubank     := $5127
.export _mmc5_bg_bank       := $512B
.export _mmc5_sl_counter    := $5203
.export _mmc5_irq_ctrl      := $5204

.segment	"INIT"

.interruptor preset_nmi, 30
.proc preset_nmi
	lda _vc_nt_mapping
	sta _mmc5_nt_mapping

	lda _vc_chr_lbank
	sta _mmc5_chr_lbank
	lda _vc_chr_ubank
	sta _mmc5_chr_ubank
	lda _vc_bg_bank
	sta _mmc5_bg_bank
	rts
.endproc

.interruptor post_nmi, 10
.proc post_nmi
    ; so many hblank variables, but worth it to handle glitch frames
	lda #<_scanline_callbacks
	sta _vc_sl_ptr
	lda #>_scanline_callbacks
	sta _vc_sl_ptr+1
	lda #0
	sta _vc_sl_i
	ldy #0
	lda (_vc_sl_ptr),y
	sta _mmc5_sl_counter
	rts
.endproc

.proc handle_hblank
	lda     _scanlineCount
	beq     done
	ldy     #$02
	lda     (_vc_sl_ptr),y
	sta     ptr1+1
	dey
	lda     (_vc_sl_ptr),y
	sta		ptr1
	dey
	lda     (_vc_sl_ptr),y
	jsr     pusha
	lda     ptr1
	ldx     ptr1+1
	jsr     callax
	lda     _vc_sl_ptr
	inc     _vc_sl_i
	clc
	adc     #3
	sta     _vc_sl_ptr
	bcc     L0001
	inc     _vc_sl_ptr+1
L0001:
	lda     _scanlineCount
	cmp     _vc_sl_i
	bne     L0042
	lda     #<(_scanline_callbacks)
	sta     _vc_sl_ptr
	lda     #>(_scanline_callbacks)
	sta     _vc_sl_ptr+1
	lda		#0
	sta		_vc_sl_i
L0042:
	ldy		#0
	lda		(_vc_sl_ptr),y
	sec
	sbc		_mmc5_sl_counter
	sta		_mmc5_sl_counter
	lda		#$80
	sta		_mmc5_irq_ctrl
done:
	rts
.endproc

.code

; void __fastcall__ vm_SetNametableMirroring(char code)
.proc _vm_SetNametableMirroring
	sta _vc_nt_mapping
	rts
.endproc

; void __fastcall__ vbm_SetNametableMirroring(char code)
.proc _vbm_SetNametableMirroring
	sta _vc_nt_mapping
	sta _mmc5_nt_mapping
	rts
.endproc
