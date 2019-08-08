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
        printf("this is idle\n");
        kdelay();
        clear_zombie();
//        printf("%d tasks in task_queue\n", task_nums());
    }
}


//kernel C entry
void init_main()
{
    disable_irq();
    init_mm();
    timer_init();

//    copy_process(USER_PROCESS);
    kthread_create(idle_main);
    copy_process(USER_PROCESS);
//    copy_process(USER_PROCESS);

    enable_irq();
    while(1){
    }
}


