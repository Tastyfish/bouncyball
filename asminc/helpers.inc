.macro inc16 addr
	.local done
	inc addr
	bne done
	inc addr+1
done:
.endmacro

.macro dec16a addr
	.local done
	dec addr
	lda addr
	cmp #$FF
	bne done
	dec addr+1
done:
.endmacro

.macro dec16x addr
	.local done
	dec addr
	ldx addr
	cpx #$FF
	bne done
	dec addr+1
done:
.endmacro

.macro cmp16a addr, op, val
	.local done
	.if(.match(.left(1, {val}), #))
		lda #<(.right(.tcount({val})-1, {val}))
		cmp addr
		op done
		lda #>(.right(.tcount({val})-1, {val})
		cmp addr+1
	.else
		lda val
		cmp addr
		op done
		lda val+1
		cmp addr+1
	.endif
done:
.endmacro

.macro cmp16x addr, op, val
	.local done
	.if(.match(.left(1, {val}), #))
		ldx #<(.right(.tcount({val})-1, {val}))
		cpx addr
		op done
		ldx #>(.right(.tcount({val})-1, {val})
		cpx addr+1
	.else
		ldx val
		cpx addr
		op done
		ldx val+1
		cpx addr+1
	.endif
done:
.endmacro
