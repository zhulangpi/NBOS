#include "../include/reg.h"
#include "../kernel/task.h"


void task0_main(unsigned long para);
void task1_main(unsigned long para);
void init_main(unsigned long para);



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










struct task_stack stack0 __attribute__((section(".stacks")));
struct task_struct task0 = { 
    .cpu_context = {
        .sp = (unsigned long)&stack0.stack[STACK_SZ-1],
        .pc = (unsigned long)task0_main,
    },
    .task = task0_main,
    .next = NULL,
};

struct task_stack stack0 = {
    .task_struct = &task0,
};

struct task_stack stack1 __attribute__((section(".stacks")));
struct task_struct task1 = { 
    .cpu_context = {
        .sp = (unsigned long)&stack1.stack[STACK_SZ-1],
        .pc = (unsigned long)task1_main,
    },
    .task = task1_main,
    .next = NULL,
};

struct task_stack stack1 = {
    .task_struct = &task1,
};

int puts(const char *str)
{
	while (*str)
		UART_DATA = *str++;
	return 0;
}

void init_main(unsigned long para)
{

    puts("Hello1\n");

    __switch_to(&task0);

	puts("Hello2\n");
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


