#include "aarch64.h"
#include "task.h"
#include "mm.h"
#include "lib.h"
#include "timer.h"
#include "syscall.h"
#include "printf.h"
#include "board.h"
#include "list.h"

char stack_init_task[STACK_SZ] __attribute__((section(".data.init_stack")));
//task 0 and the first task in task_queue
struct task_struct *init_task;

static void init_init_task(void);

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
    init_init_task();
    init_mm();
    timer_init();

    kthread_create(idle_main);
    copy_process(USER_PROCESS, PFLASH1_BASE, 16<<10);
    copy_process(USER_PROCESS, PFLASH1_BASE + (16<<10), 16<<10);

    enable_irq();
    while(1){
    }
}


static void init_init_task(void)
{
    init_task = (struct task_struct*)stack_init_task;
    INIT_LIST_HEAD(&init_task->list);
}

