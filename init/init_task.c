#include "aarch64.h"
#include "task.h"
#include "mm.h"
#include "lib.h"
#include "timer.h"
#include "syscall.h"
#include "printf.h"
#include "board.h"
#include "list.h"
#include "soft_timer.h"
#include "fs.h"


char stack_init_task[STACK_SZ] __attribute__((section(".data.init_stack")));
//task 0 and the first task in task_queue
struct task_struct *init_task;

static void init_init_task(void);

void idle_main(void)
{
    while(1){
        printf("this is idle\n");
        kdelay();
//        walk_timer_list();
//        clear_zombie();
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
    init_timer();

    init_fs();

    print_root_bdev();
    
    kthread_create(idle_main);
    sys_execv("/user_code.bin");
//    print_task_struct(current);

    //打开中断后，周期定时器中断中就可以开始调度了，以下均可能被抢占
    enable_irq();
    while(1){
        printf("this is init\n");
        kdelay();
    }
}


//初始化init_task的任务描述符，以便于可以被调度器正确调度
static void init_init_task(void)
{
    init_task = (struct task_struct*)stack_init_task;
    INIT_LIST_HEAD(&init_task->list);
    init_task->state = TASK_RUNNING;
    init_task->canary = CANARY_MAGIC_NUM;
}

