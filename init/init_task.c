#include "reg.h"
#include "task.h"
#include "mm.h"
#include "lib.h"

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


/* Exception SVC Test */
void exception_svc(void)
{
/*
Supervisor call to allow application code to call the OS. 
    It generates an exception targeting exception level 1 (EL1).
*/
    asm("svc #0xdead");
}


void init_main(unsigned long para)
{
    puts("enter init_main\n");
    init_heap();

    exception_svc();
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


