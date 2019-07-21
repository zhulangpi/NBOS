#define _ASSEMBLY_
#include "asm/arm_asm.h"
#include "syscall.h"

/* creates section containing entry code */
    .section CODE_ENTRY, "x"
/* exporting the name to the linker */
    .globl  _start
    text_align
_start:
    //MMU
//    ldr x1, =0x3520 //4GB空间 4KB粒度 Inner-shareable
//    msr tcr_el1, x1

//    ldr x1, =0xffffffff
//    msr mair_el1, x1

//    adr x0, ttb0_base
//    msr ttbr0_el1, x0


    //Initialize VBAR_EL1
    ldr x0, =vector_table_el1
    msr vbar_el1,   x0

    /* configure init kernel task stack */
    adrp    x0, __init_stack_top
    mov     sp, x0          //sp_el1

    b       init_main



/*
    任务A总是在内核态切换为任务B，切换是一个子函数，子函数返回时，只需要恢复x19-x29，x30(lr)，因此只需要保存这些再加上sp
   
    下一次调度到A时，就是恢复x19-x30,sp，就可以返回到调用cpu_switch_to的函数f，f是使用A的内核栈的内核代码
 
    sp_el0的保存恢复由对应任务的陷入内核的异常来保存，因为这些寄存器是恢复异常时需要的

    所以任务切换是在内核处理途中插入代码完成的，而不是必须在内核返回到用户态时进行的
*/

    .globl  cpu_switch_to
cpu_switch_to:
    mov x10, #0   //offsetof( struct task_struct, cpu_context) 
    add x8, x0, x10             //&(next->cpu_context)
    mov x9, sp
    stp x19, x20, [x8], #16     // store callee-saved registers
    stp x21, x22, [x8], #16 
    stp x23, x24, [x8], #16 
    stp x25, x26, [x8], #16 
    stp x27, x28, [x8], #16 
    stp x29, x9, [x8], #16 
    str x30, [x8]
    add x8, x1, x10 
    ldp x19, x20, [x8], #16     // restore callee-saved registers
    ldp x21, x22, [x8], #16 
    ldp x23, x24, [x8], #16 
    ldp x25, x26, [x8], #16 
    ldp x27, x28, [x8], #16 
    ldp x29, x9, [x8], #16 
    ldr x30, [x8]
    mov sp, x9
    ret
