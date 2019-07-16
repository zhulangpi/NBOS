#include "asm/arm_asm.h"



//异常可能发生在任意执行点，所有寄存器都要保存，还需要考虑异常入口是el0还是el1
//对于el0->el1的异常，需要保存
//x0-x30, sp_el0, elr_el1 到 el0任务的cpu_context中，这是会了回到任务
//还需要保存 spsr_el1,esr_el1


.macro exception_handler
    /*
     *  保存x30-x0到内核栈
     */
    str x30, [sp, #-8]!
    stp x29, x28, [sp, #-16]!
    stp x27, x26, [sp, #-16]!
    stp x25, x24, [sp, #-16]!
    stp x23, x22, [sp, #-16]!
    stp x21, x20, [sp, #-16]!
    stp x19, x18, [sp, #-16]!
    stp x17, x16, [sp, #-16]!
    stp x15, x14, [sp, #-16]!
    stp x13, x12, [sp, #-16]!
    stp x11, x10, [sp, #-16]!
    stp x9, x8, [sp, #-16]!
    stp x7, x6, [sp, #-16]!
    stp x5, x4, [sp, #-16]!
    stp x3, x2, [sp, #-16]!
    stp x1, x0, [sp, #-16]!
    //先通过sp_el1 获得 task_struct
    mov x0, sp
    movn x1, #(4096-1)
    and x0, x0, x1      //x0 = & task_struct
    mov x1, #0          //offsetof( struct task_struct, cpu_context)
    add x0, x0, x1      //x0 = & current->cpu_context
    //恢复内核栈中的x0-x30到陷入任务的cpu_context，栈顶是x0，逆序保存进cpu_context
    ldp x11, x12, [sp], #16         //x0,x1出栈
    stp x11, x12, [x0], #16         //保存x0,x1进cpu_context
    //x2,x3
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x4,x5
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x6,x7
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x8,x9
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x10,x11
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x12,x13
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x14,x15
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x16,x17
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x18,x19
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x20,x21
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x22,x23
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x24,x25
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x26,x27
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x28,x29
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x30
    ldr x11, [sp], #8
    str x11, [x0], #8
    //寄存器出栈并保存完毕，sp在栈底，x0指向context->sp
    //保存sp_el0, elr_el1到cpu_context
    mrs x11, sp_el0
    str x11, [x0], #8
    mrs x11, elr_el1
    str x11, [x0], #8
    //保存spsr_el1, esr_el1到栈底
    mrs x2,  esr_el1
    mrs x3,  spsr_el1
    stp x2, x3, [sp, #-16]!
    
    //传递异常栈帧给异常处理函数，栈帧中保存着esr_el1，异常处理函数会用到
    mov x0,  sp
    //异常处理
//    bl  handler

    //从栈中恢复spsr_el1，esr_el1丢弃
    ldr x3, [sp], #8
    msr spsr_el1, x3

//切换到某个任务上下文
    b   schelude   //实际不会返回
    
    eret

.endm





    vector_table_align
    .globl vector_table_el1
vector_table_el1:
    /*
     * Current EL with SP0
     */
    vector_entry_align
    b   _curr_el_sp0_sync       /* Synchronous */
    vector_entry_align
    b   _curr_el_sp0_irq        /* IRQ/vIRQ */
    vector_entry_align
    b   _curr_el_sp0_fiq        /* FIQ/vFIQ */
    vector_entry_align
    b   _curr_el_sp0_serror     /* SError/vSError */

    /*
     * Current EL with SPx
     */
    vector_entry_align
    b   _curr_el_spx_sync       /* Synchronous */
    vector_entry_align
    b   _curr_el_spx_irq        /* IRQ/vIRQ */
    vector_entry_align
    b   _curr_el_spx_fiq        /* FIQ/vFIQ */
    vector_entry_align
    b   _curr_el_spx_serror     /* SError/vSError */

    /*
     * Lower EL using AArch64
     */
    vector_entry_align
    b   _lower_el_aarch64_sync
    vector_entry_align
    b   _lower_el_aarch64_irq
    vector_entry_align
    b   _lower_el_aarch64_fiq
    vector_entry_align
    b   _lower_el_aarch64_serror

    /*
     * Lower EL using AArch32
     */
    vector_entry_align
    b   _lower_el_aarch32_sync
    vector_entry_align
    b   _lower_el_aarch32_irq
    vector_entry_align
    b   _lower_el_aarch32_fiq
    vector_entry_align
    b   _lower_el_aarch32_serror



    text_align
_curr_el_sp0_sync:
    b   .

    text_align
_curr_el_sp0_irq:
    b   .

    text_align
_curr_el_sp0_fiq:
    b   .
    text_align
_curr_el_sp0_serror:
    b   .

    text_align
_curr_el_spx_sync:
    b   .

    text_align
_curr_el_spx_irq:
    b   .

    text_align
_curr_el_spx_fiq:
    b   .
    text_align
_curr_el_spx_serror:
    b   .


//el0进入的异常
    text_align
_lower_el_aarch64_sync:
//    exception_handler
    /*
     *  保存x30-x0到内核栈
     */
    str x30, [sp, #-8]!
    stp x29, x28, [sp, #-16]!
    stp x27, x26, [sp, #-16]!
    stp x25, x24, [sp, #-16]!
    stp x23, x22, [sp, #-16]!
    stp x21, x20, [sp, #-16]!
    stp x19, x18, [sp, #-16]!
    stp x17, x16, [sp, #-16]!
    stp x15, x14, [sp, #-16]!
    stp x13, x12, [sp, #-16]!
    stp x11, x10, [sp, #-16]!
    stp x9, x8, [sp, #-16]!
    stp x7, x6, [sp, #-16]!
    stp x5, x4, [sp, #-16]!
    stp x3, x2, [sp, #-16]!
    stp x1, x0, [sp, #-16]!
    //先通过sp_el1 获得 task_struct
    mov x0, sp
    movn x1, #(4096-1)
    and x0, x0, x1      //x0 = & task_struct
    mov x1, #0          //offsetof( struct task_struct, cpu_context)
    add x0, x0, x1      //x0 = & current->cpu_context
    //恢复内核栈中的x0-x30到陷入任务的cpu_context，栈顶是x0，逆序保存进cpu_context
    ldp x11, x12, [sp], #16         //x0,x1出栈
    stp x11, x12, [x0], #16         //保存x0,x1进cpu_context
    //x2,x3
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x4,x5
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x6,x7
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x8,x9
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x10,x11
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x12,x13
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x14,x15
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x16,x17
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x18,x19
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x20,x21
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x22,x23
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x24,x25
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x26,x27
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x28,x29
    ldp x11, x12, [sp], #16
    stp x11, x12, [x0], #16
    //x30
    ldr x11, [sp], #8
    str x11, [x0], #8
    //寄存器出栈并保存完毕，sp在栈底，x0指向context->sp
    //保存sp_el0, elr_el1到cpu_context
    mrs x11, sp_el0
    str x11, [x0], #8
    mrs x11, elr_el1
    str x11, [x0], #8
    //保存spsr_el1, esr_el1到栈底
    mrs x2,  esr_el1
    mrs x3,  spsr_el1
    stp x2, x3, [sp, #-16]!

    //传递异常栈帧给异常处理函数，栈帧中保存着esr_el1，异常处理函数会用到
    mov x0,  sp
    //异常处理
//    bl  handler

    //从栈中恢复spsr_el1，esr_el1丢弃
    ldr x3, [sp], #8
    ldr x3, [sp], #8
    msr spsr_el1, x3

//切换到某个任务上下文
    b   schelude   //实际不会返回

    eret




    text_align
_lower_el_aarch64_irq:
    b   .
    text_align
_lower_el_aarch64_fiq:
    b   .
    text_align
_lower_el_aarch64_serror:
    b   .


    text_align
_lower_el_aarch32_sync:
    b   .
    text_align
_lower_el_aarch32_irq:
    b   .
    text_align
_lower_el_aarch32_fiq:
    b   .
    text_align
_lower_el_aarch32_serror:
    b   .



