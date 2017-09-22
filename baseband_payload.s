	.text 0x404F0958
	.arm
main:
/* check for already applied */
	LDR R0, =0x203C182C
	LDR R0, [R0]
	CMP R0, #0
	BEQ done

/* interrupt disable */
	MRS R0, CPSR
	ORR R0, R0, #0xC0
	MSR CPSR_c, R0

/* copy the page */
	LDR R0, =0x43000000
	LDR R1, =0x20300000
	LDR R2,   =0x100000
	LDR R4, =0x201436C8
	BLX R4

/* patch it */
	LDR R0, =0x430C182C
	MOV R1, #0
	STR R1, [R0]

/* MMU disable */
	MRC p15, 0, R3,c1,c0
	BIC R3, #0xFF
	MCR p15, 0, R3,c1,c0
	NOP
	NOP
	NOP
	NOP

/* Write to TLB */
	MOV R0, #0
	LDR R1, =0x10C0
	LDR R2, [R1]
	STR R0, [R1]

	LDR R1, =0x80C
	STR R2, [R1]

/* invalidate TLB */
	MCR p15, 0, R0, c8, c7, 0

/* MMU enable */
	MRC p15, 0, R4,c1,c0
	BIC R4, #0xFF
	ORR R4, #7
	MCR p15, 0, R4,c1,c0

/* flush i-cache */
	MOV R0, #0
	MCR p15, 0,  R0, c7, c5
	NOP
	NOP
	NOP

/* interrupt enable */
	MRS R0, CPSR
	BIC R0, R0, #0xC0
	MSR CPSR_c, R0

done:
/* you succeeded, go home */
	MOV R0, #0
	LDR R4, =0x20525359
	BX R4
	