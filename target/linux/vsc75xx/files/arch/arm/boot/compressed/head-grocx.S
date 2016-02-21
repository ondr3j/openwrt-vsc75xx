#include <asm/mach-types.h>

	.section	".start", "ax"

__grocx_start:
        
        @ VSC5605EV: 1956
	mov	r3,	#(MACH_TYPE_VSC5605EV & 0xff)
	orr	r3, r3, #(MACH_TYPE_VSC5605EV & 0xff00)
	cmp	r7, r3
	beq	99f
        
        @ Default to VSC750x Scutum
        mov	r7,     #(MACH_TYPE_GROCX & 0xFF00)
	orr	r7, r7, #(MACH_TYPE_GROCX & 0x00FF)

99:
