.include "zeropage.inc"
.include "mmc5zp.inc"

.macpack	longbranch
.import		_mmc5_sl_counter
.import		_mmc5_irq_ctrl
.import     _mmc5_nt_mapping
.import		_scanline_callbacks, _scanlineCount
.import		callax, pusha

.export		handle_hblank, _vm_SetNametableMirroring, _vbm_SetNametableMirroring

.segment	"CODE"

.proc	handle_hblank
	lda     _scanlineCount
	beq     done
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
	lda     _mmc_sl_ptr
	inc     _mmc_sl_i
	clc
	adc     #3
	sta     _mmc_sl_ptr
	bcc     L0001
	inc     _mmc_sl_ptr+1
L0001:
	lda     _scanlineCount
	cmp     _mmc_sl_i
	bne     L0042
	lda     #<(_scanline_callbacks)
	sta     _mmc_sl_ptr
	lda     #>(_scanline_callbacks)
	sta     _mmc_sl_ptr+1
	lda		#0
	sta		_mmc_sl_i
L0042:
	ldy		#0
	lda		(_mmc_sl_ptr),y
	sec
	sbc		_mmc5_sl_counter
	sta		_mmc5_sl_counter
	lda		#$80
	sta		_mmc5_irq_ctrl
done:
	rts
.endproc

; void __fastcall__ vm_SetNametableMirroring(char code)
.proc _vm_SetNametableMirroring
	sta _mmc_mirroring
	rts
.endproc

; void __fastcall__ vbm_SetNametableMirroring(char code)
.proc _vbm_SetNametableMirroring
	sta _mmc_mirroring
	sta _mmc5_nt_mapping
	rts
.endproc
