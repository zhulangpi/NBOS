#include "task.h"


extern void cpu_switch_to(struct task_struct *prev, struct task_struct *next);

void __switch_to(struct task_struct *next)
{
    struct task_struct *prev = NULL;

    prev = current;

    if(prev==next)
        return ;

    cpu_switch_to(prev,next);

}


