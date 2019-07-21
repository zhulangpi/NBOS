#ifndef _TASK_H_
#define _TASK_H_

#include "type.h"


#define STACK_SZ        (4<<10)
#define USER_STACK_SZ   (STACK_SZ)


#define KERNEL_THREAD   0
#define USER_PROCESS    1

/*
 * PSR bits
 */
#define PSR_MODE_EL0t   0x00000000
#define PSR_MODE_EL1t   0x00000004
#define PSR_MODE_EL1h   0x00000005
#define PSR_MODE_EL2t   0x00000008
#define PSR_MODE_EL2h   0x00000009
#define PSR_MODE_EL3t   0x0000000c
#define PSR_MODE_EL3h   0x0000000d



//任务状态定义
#define TASK_DEAD    0   //任务刚创建或任务被杀死
#define TASK_RUNNING 1   //任务可以运行
#define TASK_STOP    2   //任务被暂停或阻塞


struct cpu_context {
    unsigned long x19;
    unsigned long x20;
    unsigned long x21;
    unsigned long x22;
    unsigned long x23;
    unsigned long x24;
    unsigned long x25;
    unsigned long x26;
    unsigned long x27;
    unsigned long x28;
    unsigned long x29;  //fp
    unsigned long sp;   //在内核态执行任务切换时的内核栈指针
    unsigned long pc;   //x30(lr)
};

//在返回到用户态前，准备好用户态的寄存器，用于正确返回
struct pt_regs{
    unsigned long x[31];    //x0-x30
    unsigned long sp;       //sp_el0  保存用户态栈指针
    unsigned long pc;      //elr_el1 用户态程序入口
    unsigned long spsr;     //spsr_el1 机器状态，保证eret返回el0
};


#define TASK_QUEUE_LENGTH   32


/* 任务描述符 */
struct task_struct{
    struct cpu_context cpu_context;
    int state;
    unsigned long preempt_count;
};


/* 从内核栈指针得到任务描述符 */
static __always_inline struct task_struct *get_current(void)
{
    unsigned long sp;

    asm ("mov %0, sp" : "=r" (sp));

    return (struct task_struct *) ((unsigned long)(sp & (~(STACK_SZ-1))) ) ; 

}

#define current get_current()



extern void preempt_disable(void);
extern void preempt_enable(void);
extern void copy_process(unsigned long flags, void (*main)(void) );
extern void schedule(void);
extern void scheduler_tick(void);

#endif
