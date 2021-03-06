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
//    bl schedule   //在返回用户态时触发调度
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


    .macro exception_handler_entry, num
    mov x0, \num
    mrs x1, elr_el1
    mrs x2, esr_el1
    bl  put_exception
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


_curr_el_sp0_sync:
    exception_handler_entry 0
_curr_el_sp0_irq:
    exception_handler_entry 1
_curr_el_sp0_fiq:
    exception_handler_entry 2
_curr_el_sp0_serror:
    exception_handler_entry 3

_curr_el_spx_sync:
    exception_handler_entry 4

//在内核态时接收到irq，进入异常，
//不使用单独的中断栈，使用当前任务的内核栈
_curr_el_spx_irq:
    kernel_entry 1
    bl  irq_dispatch
    kernel_exit 1

_curr_el_spx_fiq:
    exception_handler_entry 6
_curr_el_spx_serror:
    exception_handler_entry 7

//el0进入的异常，系统调用
_lower_el_aarch64_sync:
    kernel_entry 0
    mrs x19, esr_el1
    and x20, x19, #ESR_EC_MASK
    mov x21, #ESR_EC_SVC
    cmp x20, x21
    b.eq  el0_svc

    //不是系统调用，错误处理
1:
    exception_handler_entry 8

el0_svc:
    ldr x9, =syscall_table
    mov x10,  #SYSCALL_NR
    cmp x8, x10
    b.hs 1b             //如果大于等于最大系统调用总数，错误处理
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
//fork出的、新加入任务队列的、从未运行过的任务 的通用入口代码，x19存储跳转地址
ret_from_fork:
    cbz x19, ret_to_user
    mov x30, x19        //just a kthread
    ret 
ret_to_user:            //not a kthread
    msr ttbr0_el1, x20
    tlbi vmalle1is
    kernel_exit 0


//el0进入的irq
_lower_el_aarch64_irq:
    kernel_entry 0
    bl irq_dispatch
    kernel_exit 0

_lower_el_aarch64_fiq:
    exception_handler_entry 10
_lower_el_aarch64_serror:
    exception_handler_entry 11

_lower_el_aarch32_sync:
    exception_handler_entry 12
_lower_el_aarch32_irq:
    exception_handler_entry 13
_lower_el_aarch32_fiq:
    exception_handler_entry 14
_lower_el_aarch32_serror:
    exception_handler_entry 15


/*
    任务A总是在内核态切换为任务B，切换是一个子函数，子函数返回时，只需要恢复x19-x29，x30(lr)，因此只需要保存这些再加上sp
   
    下一次调度到A时，就是恢复x19-x30,sp，就可以返回到调用cpu_switch_to的函数f，f是使用A的内核栈的内核代码
 
    sp_el0的保存恢复由对应任务的陷入内核的异常来保存，因为这些寄存器是恢复异常时需要的

    所以任务切换是在内核处理途中插入代码完成的，而不是必须在内核返回到用户态时进行的
*/

    .globl  cpu_switch_to
cpu_switch_to:
    mov x10, #0                 //offsetof( struct task_struct, cpu_context) 
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

