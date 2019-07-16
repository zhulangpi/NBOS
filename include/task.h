#ifndef _TASK_H_
#define _TASK_H_

#include "type.h"


#define STACK_SZ (4<<10)
#define USER_STACK_SZ   STACK_SZ


//任务状态定义
#define DEAD    0
#define RUNNING 1
#define STOP    2





//在陷入内核时，需要先保存的有
//  x0-x30      全部必须保存
//  sp_el0      需要保存，异常返回时，可能切换任务
//  elr_el1     即pc，需要保存
//  pc          任务切换时的跳转地址
//  spsr_el1    需要保存，但是应该保存到异常栈帧里，是任务无关的

struct cpu_context {
    unsigned long x0;
    unsigned long x1;
    unsigned long x2;
    unsigned long x3;
    unsigned long x4;
    unsigned long x5;
    unsigned long x6;
    unsigned long x7;
    unsigned long x8;
    unsigned long x9;
    unsigned long x10;
    unsigned long x11;
    unsigned long x12;
    unsigned long x13;
    unsigned long x14;
    unsigned long x15;
    unsigned long x16;
    unsigned long x17;
    unsigned long x18;
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
    unsigned long x30;  //lr
    unsigned long sp;   //用户态栈，调度器在允许下一个进程运行前，要先设置好sp_el0
    unsigned long pc;   //通过赋值elr_el1，eret来实现返回
};


#define TASK_QUEUE_LENGTH   32

/* 任务描述符 */
struct task_struct{
    struct cpu_context cpu_context;
    void (*main)(void);
    int state;
};


//定义一个任务
#define CREATE_TASK(name) \
    char stack_##name[STACK_SZ] __attribute__((section(".stacks"))); \
    struct task_struct *name = (struct task_struct*)stack_##name ;


/* 从内核栈指针得到任务描述符 */
static __always_inline struct task_struct *get_current(void)
{
    unsigned long sp;
    struct task_struct * curr = NULL;

    asm ("mov %0, sp" : "=r" (sp));

    curr = (struct task_struct *) (*(unsigned long *)(sp & (~(STACK_SZ-1))) ) ; 

    return curr ;
}

#define current get_current()




extern void switch_to(struct task_struct *next);
extern void task_init( struct task_struct *p, void (*main)(void) );
extern void task_add(struct task_struct *p);
extern void schelude(void);

#endif
