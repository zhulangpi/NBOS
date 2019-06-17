#include "reg.h"

/* creates section containing executable code */
    .section    INTERRUPT_VECTOR, "x"
/* exporting the name to the linker */
    .global _Reset
    .align  3
_Reset:
    B   ResetHandler /* Reset */
    B   . /* Undefined */
    B   . /* SWI */
    B   . /* Prefetch Abort */
    B   . /* Data Abort */
    B   . /* reserved */
    B   . /* IRQ */
    B   . /* FIQ */

    .text
ResetHandler:
    /* branch and save the return address in link register */
    bl  _start
    b .

    .globl	_start
_start:
	/* configure stack */
	adrp    x0, stack_top
	mov     sp, x0

	b main

