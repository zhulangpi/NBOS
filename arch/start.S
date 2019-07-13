#include "reg.h"

    .text
/* creates section containing entry code */
    .section CODE_ENTRY, "x"
/* exporting the name to the linker */
    .globl  _start
    .align  4
_start:
    //Initialize VBAR_EL1
    ldr x0, =vector_table_el1
    msr vbar_el1,   x0

    /* configure init task stack */
    adrp    x0, __init_stack_top
    mov     sp, x0

    b       init_main

// Typical exception vector table code.

    .balign 0x800
    vector_table_el1:
    curr_el_sp0_sync:   // The exception handler for the synchronous exception from the current EL using SP0.
    mov x0, #0
    b   put_exception

    .balign 0x80
    curr_el_sp0_irq:    // The exception handler for the IRQ exception from the current EL using SP0.
    mov x0, #1
    b   put_exception

    .balign 0x80
    curr_el_sp0_fiq:    // The exception handler for the FIQ exception from the current EL using SP0.
    mov x0, #2
    b   put_exception

    .balign 0x80
    curr_el_sp0_serror: // The exception handler for the system error exception from the current EL using SP0.
    mov x0, #3
    b   put_exception

//EL1级别发生的本地异常在此处理
    .balign 0x80
    curr_el_spx_sync:   // The exception handler for the synchronous exception from the current EL using the current SP.
    mov x0, #4
    b   put_exception

    .balign 0x80
    curr_el_spx_irq:    // The exception handler for IRQ exception from the current EL using the current SP.
    mov x0, #5
    bl   timer_handler
    eret

    .balign 0x80
    curr_el_spx_fiq:    // The exception handler for the FIQ exception from the current EL using the current SP.
    mov x0, #6
    b   put_exception

    .balign 0x80
    curr_el_spx_serror: // The exception handler for the system error exception from the current EL using the current SP.
    mov x0, #7
    b   put_exception



    .balign 0x80
    lower_el_aarch64_sync:  // The exception handler for the synchronous exception from a lower EL (AArch64).
    mov x0, #8
    b   put_exception

    .balign 0x80
    lower_el_aarch64_irq:   // The exception handler for the IRQ exception from a lower EL (AArch64).
    mov x0, #9
    b   put_exception

    .balign 0x80
    lower_el_aarch64_fiq:       // The exception handler for the FIQ exception from a lower EL (AArch64).
    mov x0, #10
    b   put_exception

    .balign 0x80
    lower_el_aarch64_serror:    // The exception handler for the system error exception from a lower EL(AArch64).
    mov x0, #11
    b   put_exception

    .balign 0x80
    lower_el_aarch32_sync:      // The exception handler for the synchronous exception from a lower EL(AArch32).
    mov x0, #12
    b   put_exception

    .balign 0x80
    lower_el_aarch32_irq:       // The exception handler for the IRQ exception from a lower EL (AArch32).
    mov x0, #13
    b   put_exception

    .balign 0x80
    lower_el_aarch32_fiq:       // The exception handler for the FIQ exception from a lower EL (AArch32).
    mov x0, #14
    b   put_exception

    .balign 0x80
    lower_el_aarch32_serror:    // The exception handler for the system error exception from a lower EL(AArch32)
    mov x0, #15
    b   put_exception





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


