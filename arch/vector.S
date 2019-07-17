#include "asm/arm_asm.h"


//异常可能发生在任意执行点，所有寄存器都要保存，还需要考虑异常入口是el0还是el1
//对于el0->el1的异常，需要保存
//x0-x30, sp_el0, elr_el1 到 el0任务的cpu_context中，这是会了回到任务
//还需要保存 spsr_el1,esr_el1
.macro exception_handler

.endm


.macro push_x30_x0
    //保存x30-x0到内核栈
    stp xzr, x30, [sp, #-16]!
    stp x28, x29, [sp, #-16]!
    stp x26, x27, [sp, #-16]!
    stp x24, x25, [sp, #-16]!
    stp x22, x23, [sp, #-16]!
    stp x20, x21, [sp, #-16]!
    stp x18, x19, [sp, #-16]!
    stp x16, x17, [sp, #-16]!
    stp x14, x15, [sp, #-16]!
    stp x12, x13, [sp, #-16]!
    stp x10, x11, [sp, #-16]!
    stp x8, x9, [sp, #-16]!
    stp x6, x7, [sp, #-16]!
    stp x4, x5, [sp, #-16]!
    stp x2, x3, [sp, #-16]!
    stp x0, x1, [sp, #-16]!
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

    mov x0, #0
    bl   put_exception
    eret

    text_align
_curr_el_sp0_irq:
    mov x0, #1
    bl   put_exception
    eret

    text_align
_curr_el_sp0_fiq:
    mov x0, #2
    bl   put_exception
    eret
    b   .
    text_align
_curr_el_sp0_serror:
    mov x0, #3
    bl   put_exception
    eret
    b   .

    text_align
_curr_el_spx_sync:
    mov x0, #4
    bl   put_exception
    eret
    b   .

    text_align
_curr_el_spx_irq:
//在内核态时接收到irq，进入异常，不执行调度
//不使用单独的中断栈，使用当前任务的内核栈
//除了保存CPU寄存器组到栈，
    push_x30_x0 
    //sp_el0, elr_el1压栈
    mrs x2, sp_el0
    mrs x3, elr_el1
    stp x2, x3, [sp, #-16]!
    //spsr_el1, esr_el1压栈
    mrs x2,  esr_el1
    mrs x3,  spsr_el1
    stp x2, x3, [sp, #-16]!

    //中断分发
    bl  irq_dispatch

    //从栈中恢复spsr_el1，esr_el1
    ldp x2, x3, [sp], #16
    msr esr_el1,  x2
    msr spsr_el1, x3
    //sp_el0,elr_el1出栈
    ldp x2, x3, [sp], #16
    msr sp_el0,   x2
    msr elr_el1,  x3

    eret


    text_align
_curr_el_spx_fiq:
    mov x0, #6
    bl   put_exception
    eret
    b   .
    text_align
_curr_el_spx_serror:
    mov x0, #7
    bl   put_exception
    eret
    b   .


//el0进入的异常
    text_align
_lower_el_aarch64_sync:
    push_x30_x0
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
    //xzr,x30
    ldp x11, x12, [sp], #16
    str x12, [x0], #8
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

    //传递esr_el1给异常处理函数
    mov x0,  x2
    //异常分发
    bl  exception_dispatch

    //从栈中恢复spsr_el1，esr_el1丢弃，ARMv8 SP要求16字节对齐
    ldp x2, x3, [sp], #16
    msr spsr_el1, x3

//切换到某个任务上下文
    bl   schelude_core      //实际不会返回
    eret


    text_align
_lower_el_aarch64_irq:
    //用户态接收到irq
    push_x30_x0
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
    //xzr,x30
    ldp x11, x12, [sp], #16
    str x12, [x0], #8
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

    //中断分发
    bl  irq_dispatch

    //从栈中恢复spsr_el1，esr_el1丢弃，aarch64 sp要求16字节对齐
    ldp x2, x3, [sp], #16
    msr spsr_el1, x3

//切换到某个任务上下文
    bl   schelude_core   //实际不会返回
    eret

    text_align
_lower_el_aarch64_fiq:
    mov x0, #10
    bl   put_exception
    eret
    b   .
    text_align
_lower_el_aarch64_serror:
    mov x0, #11
    bl   put_exception
    eret
    b   .


    text_align
_lower_el_aarch32_sync:
    mov x0, #12
    bl   put_exception
    eret
    b   .
    text_align
_lower_el_aarch32_irq:
    mov x0, #13
    bl   put_exception
    eret
    b   .
    text_align
_lower_el_aarch32_fiq:
    mov x0, #14
    bl   put_exception
    eret
    b   .
    text_align
_lower_el_aarch32_serror:
    mov x0, #15
    bl   put_exception
    eret
    b   .



