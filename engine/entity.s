_ENTITY_EXPORT = 1
.include "helpers.inc"
.include "mem.inc"
.include "entity.inc"

.import _bzero

.export e_Create, e_Destroy, e_Draw, e_Update
.export e_DrawAll, e_UpdateAll, e_Reset

.bss

MAX_ENTITIES = 255 / .sizeof(Entity)
entity_table_SIZE = MAX_ENTITIES * .sizeof(Entity)

entity_table:
	.repeat MAX_ENTITIES
		.tag Entity
	.endrepeat

.code

; Reset entity system, hard kills all entities with no cleanup
; void e_Reset()
.proc e_Reset
	; Just clear entity table
	lda #<entity_table
	ldx #>entity_table
	jsr pushax
	lda #<entity_table_SIZE
	ldx #>entity_table_SIZE
	jmp _bzero
.endproc

; Create an entity
; Entity* e_Create(void (*ctor)(Entity*))
.proc e_Create
ctor		= 3
ent_addr	= 1
	jsr pphax ; alloc ent_addr
	
	; find a free slot
	
	ldy #$00
floop:
	cmp entity_table+Entity::DestroyCallback,y
	bne floopb
	cmp entity_table+Entity::DestroyCallback+1,y
	beq found
	
floopb:
	; didn't get a match
	clc
	tya
	adc #(.sizeof(Entity))
	tay
	cpy #entity_table_SIZE
	bne floop
	; we ran out of slots, abort
	lda #0
	ldy #ent_addr
	sta (sp),y
	dey
	sta (sp),y
	beq done
	
found:
	; we found one, get actual address
	clc
	tya
	adc #<entity_table
	ldy #ent_addr
	sta (sp),y
	lda #$00
	adc #>entity_table
	dey
	sta (sp),y
	
	; Clone ent_addr on param stack for ctor call
	tax
	iny
	lda (sp),y
	jsr pphax
	
	; Call ctor
	ldy #ctor-1
	lda (sp),y
	pha
	iny
	lda (sp),y
	sec
	sbc #$01
	pha
	rts
	
done:
	ldy #ent_addr-1
	lda (sp),y
	tax
	iny
	lda (sp),y
	pha
	ldy #4
	jsr addysp
	pla
	rts
.endproc

; Destroy an entity
; void e_Destroy(Entity* ent)
.proc e_Destroy
ent = 1
	; copy ent to addr1
	ldy #ent
	lda (sp),y
	sta addr1
	dey
	lda (sp),y
	sta addr1+1

	lda addr1
	pha
	lda addr1+1
	pha
	
	; Call the destructor
	ldy #Entity::DestroyCallback+1
	lda (addr1),y
	pha
	dey
	lda (addr1),y
	sec
	sbc #$01
	pha
	rts
	
	pla
	sta addr1+1
	pla
	sta addr1
	
	; Then, wipe it to mark unused
	lda #$00
	ldy #Entity::DestroyCallback+1
	sta (addr1),y
	dey
	sta (addr1),y
	
	ldy #2
	jmp addysp
.endproc

; Call the update callback on entity
; void e_Update(Entity* ent)
.proc e_Update
ent = 1
	; copy ent to addr1
	ldy #ent
	lda (sp),y
	sta addr1
	dey
	lda (sp),y
	sta addr1+1
	
	ldy #Entity::UpdateCallback+1
	lda (addr1),y
	pha
	dey
	lda (addr1),y
	sec
	sbc #$01
	pha
	rts
	
	ldy #2
	jmp addysp
.endproc


; Call the draw callback on entity
; void e_Draw(Entity* ent)
.proc e_Draw
ent = 1
	; copy ent to addr1
	ldy #ent
	lda (sp),y
	sta addr1
	dey
	lda (sp),y
	sta addr1+1

	ldy #Entity::DrawCallback+1
	lda (addr1),y
	pha
	dey
	lda (addr1),y
	sec
	sbc #$01
	pha
	rts
	
	ldy #2
	jmp addysp
.endproc

; Draw all entities
; void e_DrawAll()
.proc e_DrawAll
	ldy #$00
floop:
	cmp entity_table+Entity::DestroyCallback,y
	bne callme
	cmp entity_table+Entity::DestroyCallback+1,y
	beq floopb
	
callme:
	; found an active entity slot,
	; call Draw on it
	tya
	pha
	
	clc
	adc #<entity_table
	jsr ppha
	lda #$00
	adc #>entity_table
	jsr ppha
	jsr e_Draw
	
	pla
	tay
	
floopb:
	clc
	tya
	adc #(.sizeof(Entity))
	tay
	cpy #entity_table_SIZE
	bne floop
	
	rts
.endproc

; Update all entities
; voi e_UpdateAll()
.proc e_UpdateAll
	ldy #$00
floop:
	cmp entity_table+Entity::DestroyCallback,y
	bne callme
	cmp entity_table+Entity::DestroyCallback+1,y
	beq floopb
	
callme:
	; found an active entity slot,
	; call Update on it
	tya
	pha
	
	clc
	adc #<entity_table
	jsr ppha
	lda #$00
	adc #>entity_table
	jsr ppha
	jsr e_Update
	
	pla
	tay
	
floopb:
	clc
	tya
	adc #(.sizeof(Entity))
	tay
	cpy #entity_table_SIZE
	bne floop
	
	rts
.endproc

