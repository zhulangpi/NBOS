#include "type.h"
#include "aarch64.h"
#include "task.h"
#include "lib.h"
#include "mm.h"
#include "syscall.h"


extern void cpu_switch_to(struct task_struct *prev,  struct task_struct *next );
extern void ret_from_fork(void);
extern struct task_struct *init_task;


struct task_struct* task_queue[TASK_QUEUE_LENGTH] = {0};
struct task_struct* next = NULL;


void preempt_disable(void)
{
    current->preempt_count++;
}

void preempt_enable(void)
{
    current->preempt_count--;
}

//任务切换，运行在A任务的内核栈中，
//保存A的寄存器组，恢复B的寄存器组，
//跳转
static void switch_to(struct task_struct *next)
{
    struct task_struct *prev = current;

    preempt_disable();

    if(current==next || next==NULL||next->state!=TASK_RUNNING)
        goto out;

    //切换寄存器组
    cpu_switch_to(prev, next);  //切换到B，返回时是下一次被调度到
out:
    preempt_enable();
}


//从任务队列中选择一个状态为RUNNING的任务
static void select_next_task()
{
    int i=0;
    preempt_disable();
    for(i=0; i<TASK_QUEUE_LENGTH; i++){

        if( task_queue[i] && (task_queue[i]->state == TASK_RUNNING ) && (current!=task_queue[i]) ){
            next = task_queue[i];
            break;
        }
    }
    preempt_enable();
}


//周期调度器，选择下一个任务并抢占当前任务
void scheduler_tick(void)
{

    if(current->preempt_count>0){
       return;
    }

    select_next_task();

    enable_irq();
    switch_to(next);    //这里开中断，为了切换为任务B后能正常运行，正常接受中断，下次再调度到A时，回到中断，需要恢复关中断
    disable_irq();
}

//主调度器，主动发起调度和切换
void schedule()
{
    select_next_task();
    switch_to(next);
}


//将任务p添加到任务队列
static void task_add(struct task_struct *p)
{
    int i =0;

    for(i=0; i< TASK_QUEUE_LENGTH; i++ ){
        if( (task_queue[i]==NULL) || (task_queue[i]->state==TASK_DEAD) ){
            task_queue[i] = p;
            p->state = TASK_RUNNING;
            next = p;
            puts("add success\n");
            return;
        }
    }

    if(i==TASK_QUEUE_LENGTH){
        puts("task add failed! task queue full!\n");
        while(1);
    }
}

void copy_process(void (*main)(void))
{
    struct task_struct* p = NULL;

    preempt_disable();
    p = (struct task_struct*)get_free_page(); 

    p->cpu_context.x19 = (unsigned long)main;
    p->cpu_context.sp = (unsigned long)p + STACK_SZ;
    p->cpu_context.pc = (unsigned long)ret_from_fork;
    p->preempt_count = 1;

    task_add(p);
    preempt_enable();
}
