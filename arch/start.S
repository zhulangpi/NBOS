#include "asm/arm_asm.h"


/* creates section containing entry code */
    .section CODE_ENTRY, "x"
/* exporting the name to the linker */
    .globl  _start
    text_align
_start:
    //Initialize VBAR_EL1
    ldr x0, =vector_table_el1
    msr vbar_el1,   x0

    /* configure init kernel task stack */
    adrp    x0, __init_stack_top
    mov     sp, x0          //sp_el1

    b       init_main





/* 
    任务A通过系统调用陷入el1，发起调度，在cpu_switch_to中实现寄存器组的切换，切换到任务B，x0--prev tast_struct*, x1--next task_struct*

    A在陷入el1时就保存了A的寄存器组，此时在内核栈，寄存器组可以随便用了，直接恢复寄存器组后返回到el0中的B

    恢复过程，先从内存(cpu_context)中读取到内核栈中，再从内核栈中弹出到寄存器组，这是因为寄存器组在恢复的时候不能使用

*/

    .globl  cpu_switch_to
    text_align
cpu_switch_to:
    mov x9,    #0              //offsetof( struct task_struct, cpu_context)
    add x10, x0, x9             //x10 = &(next->cpu_context)

    //切换内核栈
    add x1,  x0, #4096
    mov sp,  x1

    //x0,x1
    ldp x11, x12, [x10], #16        //读x0,x1
    stp x11, x12, [sp, #-16]!       //x0,x1压栈
    //x2,x3
    ldp x11, x12, [x10], #16
    stp x11, x12, [sp, #-16]!
    //x4,x5
    ldp x11, x12, [x10], #16
    stp x11, x12, [sp, #-16]!
    //x6,x7
    ldp x11, x12, [x10], #16
    stp x11, x12, [sp, #-16]!
    //x8,x9
    ldp x11, x12, [x10], #16
    stp x11, x12, [sp, #-16]!
    //x10,x11
    ldp x11, x12, [x10], #16        
    stp x11, x12, [sp, #-16]!
    //x12,x13
    ldp x11, x12, [x10], #16
    stp x11, x12, [sp, #-16]!
    //x14,x15
    ldp x11, x12, [x10], #16
    stp x11, x12, [sp, #-16]!
    //x16,x17
    ldp x11, x12, [x10], #16
    stp x11, x12, [sp, #-16]!
    //x18,x19
    ldp x11, x12, [x10], #16
    stp x11, x12, [sp, #-16]!
    //x20,x21
    ldp x11, x12, [x10], #16
    stp x11, x12, [sp, #-16]!
    //x22,x23
    ldp x11, x12, [x10], #16
    stp x11, x12, [sp, #-16]!
    //x24,x25
    ldp x11, x12, [x10], #16
    stp x11, x12, [sp, #-16]!
    //x26,x27
    ldp x11, x12, [x10], #16
    stp x11, x12, [sp, #-16]!
    //x28,x29
    ldp x11, x12, [x10], #16
    stp x11, x12, [sp, #-16]!
    //x30
    ldr x11, [x10], #8
    str x11, [sp, #-8]!
    //sp
    ldr x11, [x10], #8
    msr sp_el0, x11
    //pc
    ldr x11, [x10], #8
    msr elr_el1, x11

    mrs x11, spsr_el1
    //sp,pc预设完毕，恢复x0-x30，堆栈此时存的是x30，逆序恢复
    ldr x30, [sp],  #8
    ldp x29, x28, [sp], #16
    ldp x27, x26, [sp], #16
    ldp x25, x24, [sp], #16
    ldp x23, x22, [sp], #16
    ldp x21, x20, [sp], #16
    ldp x19, x18, [sp], #16
    ldp x17, x16, [sp], #16
    ldp x15, x14, [sp], #16
    ldp x13, x12, [sp], #16
    ldp x11, x10, [sp], #16
    ldp x9,  x8,  [sp], #16
    ldp x7,  x6,  [sp], #16
    ldp x5,  x4,  [sp], #16
    ldp x3,  x2,  [sp], #16
    ldp x1,  x0,  [sp], #16
    //寄存器恢复完毕，返回到用户态任务
    eret
    .type cpu_switch_to, @function
    .size cpu_switch_to, .-cpu_switch_to


