#include "aarch64.h"
#include "task.h"
#include "mm.h"
#include "lib.h"
#include "timer.h"
#include "syscall.h"
#include "printf.h"

char stack_init_task[STACK_SZ] __attribute__((section(".data.init_stack")));
struct task_struct *init_task = (struct task_struct*)stack_init_task;

void idle_main(void)
{

    while(1){
        printf("idle\n");
//        schedule();
    }
}


void task0_main(void)
{
    while(1){
        printf("task0\n");
//        schedule();
    }
}


//内核初始化线程，初始化系统后跳转到idle用户态任务
void init_main()
{
    disable_irq();
    init_printf(NULL,putc);
    timer_init();
    copy_process(idle_main);
    copy_process(task0_main);
    enable_irq();
    while(1){
//        schedule();
    }
}


