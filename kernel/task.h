#ifndef _TASK_H_
#define _TASK_H_

#include "../include/type.h"


#define STACK_SZ (4<<10)


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
    unsigned long fp;
    unsigned long sp;
    unsigned long pc;
};


/* 任务描述符 */
struct task_struct{
    struct cpu_context cpu_context;
    void (*task)(unsigned long para);
    struct task_struct* next;
};

struct task_stack{
    union{
        struct task_struct* task_struct;
        char stack[STACK_SZ];
    };
};


/* 从sp得到任务描述符*/
static __always_inline struct task_struct *get_current(void)
{
    unsigned long sp;
    struct task_struct * curr = NULL;

    asm ("mov %0, sp" : "=r" (sp));

    curr = (struct task_struct *) (*(unsigned long *)(sp & (~(STACK_SZ-1))) ) ; 

    return curr ;
}

#define current get_current()

/* Thread switching */
extern void __switch_to(struct task_struct *next);


#endif
