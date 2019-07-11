#include "../include/reg.h"

/* creates section containing executable code */
    .section INTERRUPT_VECTOR, "x"
/* exporting the name to the linker */
    .global _Reset
    .align  4
_Reset:
    B   ResetHandler /* Reset */




    .text
ResetHandler:
    /* branch and save the return address in link register */
    bl  _start
    b .

    .globl	_start
_start:
	/* configure init stack */
	adrp    x0, __init_stack_top
	mov     sp, x0

	b       init_main


/* 
    任务A调用cpu_switch_to，切换到任务B，X0--prev tast_struct, X1--next task_struct
    lr(x30)中保存着cpu_switch_to的返回地址
    x9保存进函数时的sp，之后也保存进cpu_context
*/

    .globl  cpu_switch_to
    .align  4
cpu_switch_to:
    mov x10, #0    //获得offsetof( struct task_struct, cpu_context),暂定为0
    add x8, x0, x10             //x8 = &(prev->cpu_context)
    mov x9, sp                  //x9 = sp
    stp x19, x20, [x8], #16     // store callee-saved registers
    stp x21, x22, [x8], #16
    stp x23, x24, [x8], #16
    stp x25, x26, [x8], #16
    stp x27, x28, [x8], #16
    stp x29, x9, [x8], #16
    str x30, [x8]               // lr==x30，prev->cpu_context.pc = lr
    add x8, x1, x10
    ldp x19, x20, [x8], #16     // restore callee-saved registers
    ldp x21, x22, [x8], #16
    ldp x23, x24, [x8], #16
    ldp x25, x26, [x8], #16
    ldp x27, x28, [x8], #16
    ldp x29, x9, [x8], #16
    ldr x30, [x8]               // lr = next->cpu_context.pc
    mov sp, x9                  //设置好sp为next的堆栈
    ret                         //跳转到lr
    .type cpu_switch_to, @function
    .size cpu_switch_to, .-cpu_switch_to


