;
; Startup code for cc65 (NES version)
;
; By Chris
; based on code by by Groepaz/Hitmen <groepaz@gmx.net>
; based on code by Ullrich von Bassewitz <uz@cc65.org>
;

.include        "zeropage.inc"
.include        "nes.inc"

.export         _exit
.export         __STARTUP__ : absolute = 1      ; Mark as startup
.import		initlib, donelib, callmain
.import	        push0, _main, zerobss, copydata
.import         ppubuf_flush

; Linker generated symbols
.import		__RAM_START__, __RAM_SIZE__
.import		__SRAM_START__, __SRAM_SIZE__
.import		__ROM0_START__, __ROM0_SIZE__
.import		__STARTUP_LOAD__,__STARTUP_RUN__, __STARTUP_SIZE__
.import		__CODE_LOAD__,__CODE_RUN__, __CODE_SIZE__
.import		__RODATA_LOAD__,__RODATA_RUN__, __RODATA_SIZE__

.import handle_hblank
.import _scanline_callbacks

; a redo of CRT0 for our mapper/NMI/IRQ needs


.exportzp _VBLANK_FLAG      := $70
.exportzp _mmc_scrollx      := $77
.exportzp _mmc_scrolly  	:= $78
.exportzp _mmc_ctrl         := $79
.exportzp _mmc_sl_ptr       := $80
.exportzp _mmc_mirroring    := $82
.exportzp _mmc_sl_i         := $83
.export _mmc5_chr_mode      := $5101
.export _mmc5_nt_mapping    := $5105
.export _mmc5_sl_counter    := $5203
.export _mmc5_irq_ctrl      := $5204

.segment "HEADER"

.byte   $4e,$45,$53,$1a	; "NES"^Z
.byte   2	        ; ines prg  - Specifies the number of 16k prg banks.
.byte   1               ; ines chr  - Specifies the number of 8k chr banks.
.byte   %01010011       ; ines mir  - Specifies VRAM mirroring of the banks.
.byte   %00000000       ; ines map  - Specifies the NES mapper used.
.byte   0,0,0,0,0,0,0,0	; 8 zeroes

.segment "STARTUP"

; setup the CPU and System-IRQ
start:
    sei
    ; set PRG banking to 32K ASAP
    lda #$00
    sta $5100
    ; and enable writing to RAM
    lda #$02
    sta $5102
    lda #$01
    sta $5103

    cld
	ldx     #0
	stx     VBLANK_FLAG

  	stx     ringread
	stx     ringwrite
	stx     ringcount

    txs

    lda     #$20
@l: sta     ringbuff,x
	sta     ringbuff+$0100,x
	sta     ringbuff+$0200,x
        inx
	bne     @l

; Clear the BSS data

    jsr	zerobss

; initialize data
	jsr	copydata

; setup the stack

    lda     #<(__SRAM_START__ + __SRAM_SIZE__)
    sta	sp
    lda	#>(__SRAM_START__ + __SRAM_SIZE__)
   	sta	sp+1            ; Set argument stack ptr

; Call module constructors

	jsr	initlib

; Push arguments and call main()

    jsr    	callmain

; Call module destructors. This is also the _exit entry.

_exit:  jsr	donelib		; Run module destructors

; Reset the NES

   	jmp start

nmi:    pha
        tya
        pha
        txa
        pha

        lda     #1
        sta     VBLANK_FLAG

        inc     tickcount
        bne     @s
        inc     tickcount+1

@s:     jsr     ppubuf_flush

        ; reset the video counter
        lda     #$20
        sta     PPU_VRAM_ADDR2
        lda     #$00
        sta     PPU_VRAM_ADDR2

		; Set scroll
        lda     _mmc_ctrl
        sta     PPU_CTRL1
		lda     _mmc_scrollx
		sta     PPU_VRAM_ADDR1
		lda     _mmc_scrolly
		sta     PPU_VRAM_ADDR1
        ; and other mmc things
        lda     _mmc_mirroring
        sta     _mmc5_nt_mapping
        ; so many hblank variables, but worth it to handle glitch frames
        lda     #<_scanline_callbacks
        sta     _mmc_sl_ptr
        lda     #>_scanline_callbacks
        sta     _mmc_sl_ptr+1
        lda     #0
        sta     _mmc_sl_i
        ldy     #0
        lda     (_mmc_sl_ptr),y
        sta     _mmc5_sl_counter

        pla
        tax
        pla
        tay
        pla
		rti

irq:
    pha
    txa
    pha
    tya
    pha

	; handle MMC5 HBLANK
	bit _mmc5_irq_ctrl
    bpl @cont
    jsr handle_hblank
@cont:
    bit $4015
    pla
    tay
    pla
    tax
    pla
irq2:
irq1:
timerirq:
    rti

.segment "VECTORS"

.word   irq2        ; $fff4 ?
.word   irq1        ; $fff6 ?
.word   timerirq    ; $fff8 ?
.word   nmi         ; $fffa vblank nmi
.word   start	    ; $fffc reset
.word	irq         ; $fffe irq / brk