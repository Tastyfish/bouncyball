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
	.import _e_Create, _e_Destroy, _e_Draw, _e_Update
	.import _e_DrawTick, _e_UpdateTick, _e_Reset
.endif
