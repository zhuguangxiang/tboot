tboot interrupt vector:
    b	reset
	ldr pc, [pc, #tos_vector + 4 - . - PIPELINE]
	ldr pc, [pc, #tos_vector + 8 - . - PIPELINE]
	ldr pc, [pc, #tos_vector + 12 - . - PIPELINE]
	ldr pc, [pc, #tos_vector + 16 - . - PIPELINE]
	ldr pc, [pc, #tos_vector + 20 - . - PIPELINE]
	ldr pc, [pc, #tos_vector + 24 - . - PIPELINE]
	ldr pc, [pc, #tos_vector + 28 - . - PIPELINE]
tos_vector:
	.long 0x30000000
    .long 0x30000004
	.long 0x30000008
	.long 0x3000000c
	.long 0x30000010
	.long 0x30000014
	.long 0x30000018
	.long 0x3000001c

is for tiny-os, which will run at 0x30000000 in RAM, while interrupt vector runs in NorFlash at 0x00000000.
