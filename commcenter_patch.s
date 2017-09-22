	.text 0x8CFD4
	.thumb
	.set issue_command_with_callback, 0x1F971
	.set send_bb_command, 0x5F5BD

/* hijack branch @ 0x1EE5E */
/*   40 F0 AD FB -> 6E F0 B9 F8 */
/* insert at "Could not call function kAppleBasebandConnectMethodRadioOn in %s" */
main:
	PUSH {R3-R7, LR}
	MOV R6, R5
	ADD R6, #0x7C
	LDR R2, [R6,#0x20]
	CMP R2, #5
	BEQ actnormal
	ADD R2, #1
	STR R2, [R6,#0x20]
/* this is the bootleg callback for the exploit */
/* R0 goes through */
	ADR R1, vector
	STR R1, [SP]
	MOV R1, R5
	LDR R2, =0x1EDA1		/* callback */
	MOV R3, #0
	LDR R4, =issue_command_with_callback
	BLX R4
	POP {R3-R7}
	POP {R3}	/* waste PC */
	LDR R3, =0x1EE89
	BX R3
/* this is the send_bb_command for at+xlck=2 */
actnormal:
	LDR R4, =send_bb_command
	BLX R4
	POP {R3-R7, PC}
	.align
vector:
	.asciz "+xlog=1,\"geohotsatitagainallptinjectzdLJ@hahaiUR \""
