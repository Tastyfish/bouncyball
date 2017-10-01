.include "zeropage.inc"
.include "video.inc"
.include "mem.inc"

.import ppuMaskCache, ppubuf_put

.export _vb_DecompressNT
.export _v_DecompressToRAM
.export rle_byte

; Adapted from RLE decompressor by Shiru (NESASM version)
; void __fastcall__ vb_DecompressNT(int ppuAddr, void* src)
.proc _vb_DecompressNT
	sta ptr1
	stx ptr1+1

	; This runs so long, disable everything
	lda #0
	sta PPU_MASK

	jsr popax
	bit PPU_STATUS
	stx PPU_ADDR
	sta PPU_ADDR

	ldy #0
	jsr rle_byte
	sta sreg
loop:
	jsr rle_byte
	cmp sreg
	beq rle_start
	sta PPU_DATA
	sta sreg+1
	bne loop
rle_start:
	jsr rle_byte
	cmp #0
	beq done
	tax
	lda sreg+1
rle_loop:
	sta PPU_DATA
	dex
	bne rle_loop
	beq loop
done:
	lda ppuMaskCache
	sta PPU_MASK
	rts
.endproc

; void __fastcall__ v_DecompressToRAM(void* dest, void* src)
.proc _v_DecompressToRAM
	sta ptr1
	stx ptr1+1

	jsr popax
	sta ptr2
	stx ptr2+1

	ldy #0
	jsr rle_byte
	sta sreg
loop:
	jsr rle_byte
	cmp sreg
	beq rle_start
	sta (ptr2),y
	inc ptr2
	bne wcont
	inc ptr2+1
wcont:
	sta sreg+1
	bne loop
	rle_start:
	jsr rle_byte
	cmp #0
	beq done
	tax
	lda sreg+1
rle_loop:
	sta (ptr2),y
	inc ptr2
	bne wcont2
	inc ptr2+1
wcont2:
	dex
	bne rle_loop
	beq loop
done:
	rts
.endproc

.proc rle_byte
	lda (ptr1),y
	inc ptr1
	bne done
	inc ptr1+1
done:
	rts
.endproc
