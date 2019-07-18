#include "task.h"
#include "mm.h"
#include "lib.h"
#include "timer.h"
#include "syscall.h"

void init_main();

char stack_init_task[STACK_SZ] __attribute__((section(".data.init_stack")));
struct task_struct *init_task = (struct task_struct*)stack_init_task;

void idle_main(void);
CREATE_TASK(idle_task);
void task0_main(void);
CREATE_TASK(task0);
void task1_main(void);
CREATE_TASK(task1);

#define RUN_TIMES 3


//内核初始化线程，初始化系统后跳转到idle用户态任务
void init_main()
{
    puts("enter init_main\n");

    init_mm();

    timer_init();

    task_init( idle_task, idle_main );
    task_add( idle_task );
    task_init( task0, task0_main );
    task_add( task0 );
    task_init( task1, task1_main );
    task_add( task1 );


    //切换到第一个用户态任务
    schedule();
	puts("end init_main\n");
    while(1);
}

//用户态任务函数
void idle_main(void)
{
    unsigned long a = RUN_TIMES;

    puts("enter idle main\n");

    while(1){
        putlu(a--);
        puts(" idle\n");
   //     syscall(0);
       // if(a==0)
         //   while(1);
    };
}


void task0_main(void)
{
    register unsigned long x0 = 1, x1 = 2, x2 = 3, x3 = 4, x4 = 5, x5 = 6, x6 = 7, x7 = 8, x8 = 9, x9 = 10;
    register unsigned long x10 = 11, x11 = 12, x12 = 13, x13 = 14, x14 = 15, x15 = 16, x16 = 17, x17 = 18, x18 = 19, x19 = 20;
    register unsigned long x20 = 21, x21 = 22, x22 = 23, x23 =24, x24 = 25, x25 = 26, x26 = 27, x27 = 28, x28 = 29, x29 = 30, x30=31;
    unsigned long a = RUN_TIMES;

    puts("enter task0\n");
    
    while(1){
        a--;
        putlu(x0);
        putlu(x1);
        putlu(x2);
        putlu(x3);
        putlu(x4);
        putlu(x5);
        putlu(x6);
        putlu(x7);
        putlu(x8);
        putlu(x9);
        putlu(x10);
        putlu(x11);
        putlu(x12);
        putlu(x13);
        putlu(x14);
        putlu(x15);
        putlu(x16);
        putlu(x17);
        putlu(x18);
        putlu(x19);
        putlu(x20);
        putlu(x21);
        putlu(x22);
        putlu(x23);
        putlu(x24);
        putlu(x25);
        putlu(x26);
        putlu(x27);

        putlu(x28);
        putlu(x29);
        putlu(x30);
        puts(" task0\n");
   //     syscall(0);

    //    if(a==0)
      //      while(1);
    }
}

void task1_main(void)
{
    unsigned long a = RUN_TIMES;
    char s[100] = "qwertyuioplkjhgfdsazxcvbnm";

    puts("enter task1\n");
    
    while(1){
        a--;
        puts(s);
        puts(" task1\n");
  //      syscall(0);
//        if(a==0)
  //          while(1);
    }
}


