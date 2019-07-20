#include "asm/arm_asm.h"


//异常可能发生在任意执行点，所有寄存器都要保存，还需要考虑异常入口是el0还是el1
//对于el0->el1的异常，需要保存
//x0-x30, sp_el0, elr_el1，spsr_el1
//对于el1->el1的异常，需要保存
//x0-x30, elr_el1，spsr_el1

//重新考虑异常的进入，不考虑任务的上下文，保存处理器上下文
//之所以临时寄存器选用x21~，是因为例如系统调用会把参数放在x0-x7寄存器中
    .macro kernel_entry, el
    stp x0, x1, [sp, #-16]!     //满递减栈，先减地址，再写
    stp x2, x3, [sp, #-16]!
    stp x4, x5, [sp, #-16]!
    stp x6, x7, [sp, #-16]!
    stp x8, x9, [sp, #-16]!
    stp x10, x11, [sp, #-16]!
    stp x12, x13, [sp, #-16]!
    stp x14, x15, [sp, #-16]!
    stp x16, x17, [sp, #-16]!
    stp x18, x19, [sp, #-16]!
    stp x20, x21, [sp, #-16]!
    stp x22, x23, [sp, #-16]!
    stp x24, x25, [sp, #-16]!
    stp x26, x27, [sp, #-16]!
    stp x28, x29, [sp, #-16]!

    .if \el == 0  //这是从el0进入的异常，需要保存sp_el0，确保此次异常返回的正确性
    mrs x21, sp_el0
//    .else   //这是在内核态发生的异常嵌套，不需要保存sp_el0，但是保存此次异常的栈帧首地址
    add x21, sp, #(15*16)
    .endif

    stp x30, x21, [sp, #-16]!
    
    mrs x22, elr_el1
    mrs x23, spsr_el1
    stp x22, x23, [sp, #-16]!
    .endm

    .macro kernel_exit, el
    ldp x22, x23, [sp], #16
    msr elr_el1, x22
    msr spsr_el1, x23

    ldp x30, x21, [sp], #16

    .if \el == 0
    msr sp_el0, x21
    .endif 

    ldp x28, x29, [sp], #16 
    ldp x26, x27, [sp], #16 
    ldp x24, x25, [sp], #16 
    ldp x22, x23, [sp], #16 
    ldp x20, x21, [sp], #16 
    ldp x18, x19, [sp], #16 
    ldp x16, x17, [sp], #16 
    ldp x14, x15, [sp], #16 
    ldp x12, x13, [sp], #16 
    ldp x10, x11, [sp], #16 
    ldp x8,  x9,  [sp], #16 
    ldp x6,  x7,  [sp], #16 
    ldp x4,  x5,  [sp], #16 
    ldp x2,  x3,  [sp], #16 
    ldp x0,  x1,  [sp], #16 
    eret    //返回此次异常
    .endm




    vector_table_align
    .globl vector_table_el1
vector_table_el1:
    // Current EL with SP0
    vector_entry_align
    b   _curr_el_sp0_sync       /* Synchronous */
    vector_entry_align
    b   _curr_el_sp0_irq        /* IRQ/vIRQ */
    vector_entry_align
    b   _curr_el_sp0_fiq        /* FIQ/vFIQ */
    vector_entry_align
    b   _curr_el_sp0_serror     /* SError/vSError */

    // Current EL with SPx
    vector_entry_align
    b   _curr_el_spx_sync       /* Synchronous */
    vector_entry_align
    b   _curr_el_spx_irq        /* IRQ/vIRQ */
    vector_entry_align
    b   _curr_el_spx_fiq        /* FIQ/vFIQ */
    vector_entry_align
    b   _curr_el_spx_serror     /* SError/vSError */

    // Lower EL using AArch64
    vector_entry_align
    b   _lower_el_aarch64_sync
    vector_entry_align
    b   _lower_el_aarch64_irq
    vector_entry_align
    b   _lower_el_aarch64_fiq
    vector_entry_align
    b   _lower_el_aarch64_serror

    // Lower EL using AArch32
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
    text_align
_curr_el_sp0_irq:
    mov x0, #1
    bl   put_exception
    text_align
_curr_el_sp0_fiq:
    mov x0, #2
    bl   put_exception
    text_align
_curr_el_sp0_serror:
    mov x0, #3
    bl   put_exception


    text_align
_curr_el_spx_sync:
    mov x0, #4
    bl   put_exception

//在内核态时接收到irq，进入异常，
//不使用单独的中断栈，使用当前任务的内核栈
    text_align
_curr_el_spx_irq:
    kernel_entry 1
    bl  irq_dispatch
    kernel_exit 1

    text_align
_curr_el_spx_fiq:
    mov x0, #6
    bl   put_exception
    text_align
_curr_el_spx_serror:
    mov x0, #7
    bl   put_exception


//el0进入的异常，系统调用
    text_align
_lower_el_aarch64_sync:
    kernel_entry 0
    mrs x0, esr_el1
    bl exception_dispatch
    kernel_exit 0

//el0进入的irq
    text_align
_lower_el_aarch64_irq:
    kernel_entry 0
    bl irq_dispatch
    kernel_exit 0

    text_align
_lower_el_aarch64_fiq:
    mov x0, #10
    bl   put_exception
    text_align
_lower_el_aarch64_serror:
    mov x0, #11
    bl   put_exception


    text_align
_lower_el_aarch32_sync:
    mov x0, #12
    bl   put_exception
    text_align
_lower_el_aarch32_irq:
    mov x0, #13
    bl   put_exception
    text_align
_lower_el_aarch32_fiq:
    mov x0, #14
    bl   put_exception
    text_align
_lower_el_aarch32_serror:
    mov x0, #15
    bl   put_exception

