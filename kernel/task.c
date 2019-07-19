#include "type.h"
#include "task.h"
#include "lib.h"
#include "mm.h"
#include "syscall.h"

extern void cpu_switch_to( struct task_struct *next );


struct task_struct* task_queue[TASK_QUEUE_LENGTH] = {0};

struct task_struct* next = NULL;



//任务切换，运行在A任务的内核栈中，
//保存A的寄存器组，恢复B的寄存器组，
//跳转
void switch_to(struct task_struct *next)
{
    while( next->state!=RUNNING ){
        scheduler_tick();
    }
    //切换寄存器组
    cpu_switch_to(next);

}

//任务数据结构的初始化
//初始化完毕就可以参与调度，直接运行了
void task_init( struct task_struct *p, void (*main)(void) )
{
    //分配用户栈
    p->cpu_context.sp = (unsigned long)kmalloc(USER_STACK_SZ);
    if(p->cpu_context.sp==(unsigned long)NULL){
        puts("user stack malloc failed\n");
        while(1);
    }
    //满递减栈
    p->cpu_context.sp += USER_STACK_SZ;
    //设置主函数
    p->main = main;
    //设置寄存器组初值
    p->cpu_context.pc = (unsigned long)main;


}

//将任务p添加到任务队列
void task_add(struct task_struct *p)
{
    int i =0;

    for(i=0; i< TASK_QUEUE_LENGTH; i++ ){
        if( task_queue[i] == p ){
            next = p;
            return;     //已在队列中
        }
    }

    for(i=0; i< TASK_QUEUE_LENGTH; i++ ){
        if( (task_queue[i]==NULL) || (task_queue[i]->state==DEAD) ){
            task_queue[i] = p;
            p->state = RUNNING;
            next = p;
            puts("add success\n");
            return ;
        }

    }
    if(i==TASK_QUEUE_LENGTH){
        puts("task add failed! task queue full!\n");
        while(1);
    }

}

//周期调度器
//从任务队列中选择一个状态为RUNNING的任务
void scheduler_tick(void)
{
    int i = 0;
    static int idx = 0 ;

   // puts("schelude\n");

    for(i=0; i<TASK_QUEUE_LENGTH; i++){
        idx++;
        if(idx == TASK_QUEUE_LENGTH)
            idx = 0;

        if( task_queue[idx]->state == RUNNING ){
            next = task_queue[idx];
            break;
        }
    }

}

//主调度器
//调度执行，只在异常返回用户态时才允许调用
void schedule()
{
    while( next->state!=RUNNING ){
        scheduler_tick();
    }
    //切换寄存器组
    switch_to(next);

}

