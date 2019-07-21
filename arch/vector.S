#define _ASSEMBLY_
#include "asm/arm_asm.h"
#include "exception.h"
#include "syscall.h"

//异常可能发生在任意执行点，所有寄存器都要保存，还需要考虑异常入口是el0还是el1
//对于el0->el1的异常，需要保存
//x0-x30, sp_el0, elr_el1，spsr_el1
//对于el1->el1的异常，需要保存
//x0-x30, elr_el1，spsr_el1

//重新考虑异常的进入，不考虑任务的上下文，保存处理器上下文
//之所以临时寄存器选用x21~，是因为例如系统调用会把参数放在x0-x7寄存器中
    .macro kernel_entry, el
    sub sp, sp, #FRAME_SZ
    stp x0, x1, [sp, #16 * 0]
    stp x2, x3, [sp, #16 * 1]
    stp x4, x5, [sp, #16 * 2]
    stp x6, x7, [sp, #16 * 3]
    stp x8, x9, [sp, #16 * 4]
    stp x10, x11, [sp, #16 * 5]
    stp x12, x13, [sp, #16 * 6]
    stp x14, x15, [sp, #16 * 7]
    stp x16, x17, [sp, #16 * 8]
    stp x18, x19, [sp, #16 * 9]
    stp x20, x21, [sp, #16 * 10] 
    stp x22, x23, [sp, #16 * 11] 
    stp x24, x25, [sp, #16 * 12] 
    stp x26, x27, [sp, #16 * 13] 
    stp x28, x29, [sp, #16 * 14] 


    .if \el == 0    //这是从el0进入的异常，需要保存sp_el0，确保此次异常返回的正确性
    mrs x21, sp_el0
    .else           //这是在内核态发生的异常嵌套，不需要保存sp_el0，但是保存此次异常的栈帧首地址
    add x21, sp, #FRAME_SZ
    .endif

    mrs x22, elr_el1
    mrs x23, spsr_el1

    stp x30, x21, [sp, #16 * 15] 
    stp x22, x23, [sp, #16 * 16] 
    .endm

    .macro kernel_exit, el

    .if \el == 0
    bl schedule
    .endif

    ldp x22, x23, [sp, #16 * 16]
    ldp x30, x21, [sp, #16 * 15] 

    .if \el == 0
    msr sp_el0, x21
    .endif /* \el == 0 */

    msr elr_el1, x22            
    msr spsr_el1, x23


    ldp x0, x1, [sp, #16 * 0]
    ldp x2, x3, [sp, #16 * 1]
    ldp x4, x5, [sp, #16 * 2]
    ldp x6, x7, [sp, #16 * 3]
    ldp x8, x9, [sp, #16 * 4]
    ldp x10, x11, [sp, #16 * 5]
    ldp x12, x13, [sp, #16 * 6]
    ldp x14, x15, [sp, #16 * 7]
    ldp x16, x17, [sp, #16 * 8]
    ldp x18, x19, [sp, #16 * 9]
    ldp x20, x21, [sp, #16 * 10]
    ldp x22, x23, [sp, #16 * 11]
    ldp x24, x25, [sp, #16 * 12]
    ldp x26, x27, [sp, #16 * 13]
    ldp x28, x29, [sp, #16 * 14]
    add sp, sp, #FRAME_SZ
    eret
    .endm



    .globl  call_sys_write
    text_align
call_sys_write:
    mov x8, #SYS_WRITE
    svc #0
    br  x30

    .globl  call_sys_fork
call_sys_fork:
    mov x8, #SYS_FORK
    svc #0
    br  x30


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



_curr_el_sp0_sync:
 //   mrs x1, esr_el0
    mov x0, #0
    bl   put_exception
_curr_el_sp0_irq:
    mov x0, #1
    bl   put_exception
_curr_el_sp0_fiq:
    mov x0, #2
    bl   put_exception
_curr_el_sp0_serror:
    mov x0, #3
    bl   put_exception


_curr_el_spx_sync:
    mov x0, #4
    bl   put_exception

//在内核态时接收到irq，进入异常，
//不使用单独的中断栈，使用当前任务的内核栈
_curr_el_spx_irq:
    kernel_entry 1
    bl  irq_dispatch
    kernel_exit 1


_curr_el_spx_fiq:
    mov x0, #6
    bl   put_exception
_curr_el_spx_serror:
    mov x0, #7
    bl   put_exception


//el0进入的异常，系统调用
_lower_el_aarch64_sync:
    kernel_entry 0
    mrs x19, esr_el1
    and x20, x19, #ESR_EC_MASK
    mov x21, #ESR_EC_SVC
    cmp x20, x21
    b.eq  el0_svc
    mov x0, #8          //不是系统调用，错误处理
    bl  put_exception

el0_svc:
//    and x8, x19, #ESR_SVC_IMM16_MASK //获得系统调用号
    ldr x9, =syscall_table
    mov x10,  #SYSCALL_NR
    cmp x8, x10
    b.hs    .           //如果大于等于最大系统调用总数，死循环
    lsl x11, x8,  #3    //获得该系统调用函数在函数表中的偏移，一个函数槽为long，8B，所以左移3位
    add x11, x9, x11    //得到处理函数存放的地址
    ldr x12, [x11]      //得到处理函数
    bl  enable_irq
    blr x12             //系统调用处理 syscall_table[x8]
    bl  disable_irq
    //改写异常栈帧中的x0，x0保存系统调用的返回值
    add x13, sp, #FRAME_SZ
    sub x13, x13, #X0_OFFSET
    str x0,  [x13]
    kernel_exit 0

    .globl  ret_from_fork
//fork出的、新加入任务队列的、从未运行过的任务 的通用入口代码，x19存储跳转地址，任务刚创建默认关抢占，在此处打开抢占
ret_from_fork:
    cbz x19, ret_to_user    //not a kernel thread
    mov x30, x19 
    ret 
ret_to_user:
    kernel_exit 0


//el0进入的irq
_lower_el_aarch64_irq:
    kernel_entry 0
    bl irq_dispatch
    kernel_exit 0

_lower_el_aarch64_fiq:
    mov x0, #10
    bl   put_exception
_lower_el_aarch64_serror:
    mov x0, #11
    bl   put_exception


_lower_el_aarch32_sync:
    mov x0, #12
    bl   put_exception
_lower_el_aarch32_irq:
    mov x0, #13
    bl   put_exception
_lower_el_aarch32_fiq:
    mov x0, #14
    bl   put_exception
_lower_el_aarch32_serror:
    mov x0, #15
    bl   put_exception

