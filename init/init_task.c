#include "task.h"
#include "mm.h"
#include "lib.h"
#include "timer.h"


void init_main(unsigned long);

struct task_stack init_stack __attribute__((section(".data.init_stack")));
struct task_struct init_task = {
    .cpu_context = { 
        .sp = (unsigned long)&init_stack.stack[STACK_SZ-1],
        .pc = (unsigned long)init_main,
    },
    .task = init_main,
    .next = NULL,
};

struct task_stack init_stack = { 
    .task_struct = &init_task,
};



void task0_main(unsigned long);
void task1_main(unsigned long);
CREATE_TASK(task0, task0_main);
CREATE_TASK(task1, task1_main);




void init_main(unsigned long para)
{
    puts("enter init_main\n");
    init_mm();

    timer_init();
    __switch_to(&task0);

	puts("end init_main\n");
	while (1);
}


void task0_main(unsigned long para)
{
    puts("task0\n");
    __switch_to(&task1);
    puts("back0\n");
    __switch_to(&init_task);
    
    while(1){
    }
}


void task1_main(unsigned long para)
{
    puts("task1\n");
    __switch_to(&task0);
    puts("back1\n");
    while(1){
    }
}


