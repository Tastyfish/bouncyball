.struct Entity
	DestroyCallback .addr
	UpdateCallback .addr
	DrawCallback .addr
	.union
		PW .word
		.struct
			PA .byte
			PB .byte
		.endstruct
	.endunion
.endstruct

.ifndef _ENTITY_EXPORT
	.import e_Create, e_Destroy, e_Draw, e_Update
	.import e_DrawAll, e_UpdateAll, e_Reset
.endif
