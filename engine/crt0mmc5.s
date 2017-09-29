;
; Startup code for cc65 (NES version)
;
; By Chris
; based on code by by Groepaz/Hitmen <groepaz@gmx.net>
; based on code by Ullrich von Bassewitz <uz@cc65.org>
;

.include        "zeropage.inc"
.include        "nes.inc"

.export     _exit
.export     __STARTUP__ : absolute = 1      ; Mark as startup
.import		initlib, donelib, callmain
.import     push0, _main, zerobss, copydata
.import     ppubuf_flush, condes

; Linker generated symbols
.import		__RAM_START__, __RAM_SIZE__
.import		__SRAM_START__, __SRAM_SIZE__
.import		__ROM0_START__, __ROM0_SIZE__
.import		__STARTUP_LOAD__,__STARTUP_RUN__, __STARTUP_SIZE__
.import		__CODE_LOAD__,__CODE_RUN__, __CODE_SIZE__
.import		__RODATA_LOAD__,__RODATA_RUN__, __RODATA_SIZE__

.import __INTERRUPTOR_COUNT__, __INTERRUPTOR_TABLE__

.import handle_hblank, _mmc5_irq_ctrl

; a redo of CRT0 for our mapper/NMI/IRQ needs

.exportzp _VBLANK_FLAG      := $70

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
    cld

    ; set PRG banking to 32K ASAP
    lda #$00
    sta $5100
    ; and enable writing to RAM
    lda #$02
    sta $5102
    lda #$01
    sta $5103

    lda #0
    sta ringread
    sta ringwrite
    sta ringcount

    txs

    lda #$20
@l: sta ringbuff,x
	sta ringbuff+$0100,x
	sta ringbuff+$0200,x
    inx
	bne @l

; Clear the BSS data
    jsr	zerobss

; initialize data
	jsr	copydata

; setup the stack
    lda #<(__SRAM_START__ + __SRAM_SIZE__)
    sta	sp
    lda	#>(__SRAM_START__ + __SRAM_SIZE__)
   	sta	sp+1            ; Set argument stack ptr

; Call module constructors
	jsr	initlib

; Push arguments and call main()
    jsr callmain

; Call module destructors. This is also the _exit entry.
_exit:
    jsr donelib		; Run module destructors

; Reset the NES
   	jmp start

nmi:
    pha
    tya
    pha
    txa
    pha

	lda #1
	sta VBLANK_FLAG

	inc tickcount
	bne @s
	inc tickcount+1

@s:
    ldy #<(__INTERRUPTOR_COUNT__*2)
    beq @done_nmi
    lda #<__INTERRUPTOR_TABLE__
    ldx #>__INTERRUPTOR_TABLE__
    jsr condes

@done_nmi:
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
