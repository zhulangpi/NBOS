#include "type.h"
#include "aarch64.h"
#include "task.h"
#include "lib.h"
#include "mm.h"
#include "syscall.h"
#include "printf.h"


extern void cpu_switch_to(struct task_struct *prev,  struct task_struct *next );
extern void ret_from_fork(void);
extern struct task_struct *init_task;

static struct task_struct* task_queue[TASK_QUEUE_LENGTH] = { 0};
static struct task_struct* next = NULL;


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

    if(current==next || next==NULL || next->state!=TASK_RUNNING)
        goto out;

    //切换用户态地址空间
    switch_mm(next->mm);
    //切换寄存器组
    cpu_switch_to(prev, next);  //切换到B，返回时是下一次被调度到
out:
    preempt_enable();
}


//从任务队列中选择一个状态为RUNNING的任务
static void select_next_task(void)
{
    int i=0;
    static int idx = -1;
    preempt_disable();

    for(i=0; i<TASK_QUEUE_LENGTH; i++){
        idx = (idx+1) % TASK_QUEUE_LENGTH;
        if( task_queue[idx] && (task_queue[idx]->state == TASK_RUNNING ) && (current!=task_queue[idx]) ){
            next = task_queue[idx];
            break;
        }
    }
    preempt_enable();
}


//周期调度器，选择下一个任务并抢占当前任务
void scheduler_tick(void)
{

    if(current->preempt_count>0){
        printf("can't preempt\n");
       return;
    }

    select_next_task();
//    print_task_struct( task_queue[0] );
//    printf("current: 0x%p\n", current);
    //print_process_page( task_queue[1]->mm );
    enable_irq();
    switch_to(next);    //这里开中断，为了切换为任务B后能正常运行，正常接受中断，下次再调度到A时，回到中断，需要恢复关中断
    disable_irq();
}

//主调度器，主动发起调度和切换
void schedule()
{
    if(current->preempt_count>0){
        printf("can't preempt\n");
       return;
    }
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
            printf("add success\n");
            printf("%d tasks in task_queue\n", task_nums());
            return;
        }
    }
    
    if(i==TASK_QUEUE_LENGTH){
        printf("task add failed! task queue full!\n");
        while(1);
    }
}

extern unsigned long __user_start;
const unsigned long user_start = (unsigned long)&__user_start;
extern unsigned long __user_end;
const unsigned long user_end = (unsigned long)&__user_end;

//just put new task in queue
void copy_process(unsigned long flags)
{
    struct task_struct* p = NULL;
    struct pt_regs* new = NULL; 

    preempt_disable();

    p = (struct task_struct*)get_free_page(GFP_KERNEL);
    new = (struct pt_regs*)((unsigned long)p + STACK_SZ - sizeof(struct pt_regs));

    memset(p, 0, sizeof(struct task_struct));

    p->cpu_context.pc = (unsigned long)ret_from_fork;
    p->cpu_context.x19 = 0;
    p->cpu_context.sp = (unsigned long)new;    //保留异常返回的空间
    p->preempt_count = 0;

    //分配用户态地址空间
    p->mm = (struct mm_struct*)kmalloc(sizeof(struct mm_struct));
    p->mm->pgd = __pa(get_free_page(GFP_KERNEL) );
    p->cpu_context.x20 = p->mm->pgd;
    //分配并设置用户态栈
    alloc_user_pages(USER_MAX - USER_STACK_SZ, USER_STACK_SZ, p->mm);
    new->sp = USER_MAX;
    //分配程序空间
    alloc_user_pages(USER_START, user_end-user_start, p->mm);
    new->pc = USER_START;
    //装载程序到目标用户空间
    switch_mm(p->mm);
    memcpy( USER_START, (void*)user_start, user_end-user_start );
    switch_mm(current->mm);
    new->spsr = PSR_MODE_EL0t;      //使得eret能够返回到el0

    task_add(p);
    preempt_enable();

    //print_process_page(p->mm);

}


//just put new task in queue
void kthread_create(void (*main)(void))
{
    struct task_struct* p = NULL;

    preempt_disable();
    p = (struct task_struct*)get_free_page(GFP_KERNEL);

    p->cpu_context.x19 = (unsigned long)main;
    p->cpu_context.sp = (unsigned long)p + STACK_SZ;
    p->cpu_context.pc = (unsigned long)ret_from_fork;
    p->preempt_count = 0;

    task_add(p);
    preempt_enable();
}

//删除进程
void delete_process(struct task_struct* p)
{
    int i=0;

    if(!p || p->state != TASK_ZOMBIE){
        return ;
    }

    for(i=0;i<TASK_QUEUE_LENGTH;i++){
        if(task_queue[i] == p){
            task_queue[i] = NULL;
            break;
        }
    }

    //释放用户空间映射的页
    for(i=0;i<MAX_USER_PAGES;i++){
        unsigned long pa = p->mm->vma[i].pa;
        if(pa!=0){
            free_page( __va(pa));
        }else{
            break;
        }
    }

    //TODO: 释放进程页表
    for(i=0;i<MAX_PGTBL_PAGES; i++){
        void* va = p->mm->pgtbl_page[i];
        if(va!=NULL){
            free_page(va);
        }
    }

    //释放内核栈及任务描述符
    free_page(p);
}

void clear_zombie(void)
{
    int i = 0;
    preempt_disable();
    for(i=0; i<TASK_QUEUE_LENGTH; i++){
        if(task_queue[i] && task_queue[i]->state == TASK_ZOMBIE){
            delete_process(task_queue[i]);
        }
    }
    preempt_enable();
}


int task_nums(void)
{
    int i = 0, count = 0;
    for(i=0;i<TASK_QUEUE_LENGTH;i++){
        if(task_queue[i] && task_queue[i]->state == TASK_RUNNING)
            count++;
    }
    return count;
}

void print_task_struct(struct task_struct *p)
{
    struct cpu_context c = p->cpu_context;
    printf("x19: 0x%x\t x12: 0x%x\t x21: 0x%x\t x22: 0x%x\t x23: 0x%x\n", c.x19, c.x20, c.x21, c.x22, c.x23);
    printf("x24: 0x%x\t x25: 0x%x\t x26: 0x%x\t x27: 0x%x\t x28: 0x%x\n", c.x24, c.x25, c.x26, c.x27, c.x28);
    printf("x29: 0x%x\t  sp: 0x%x\t  pc: 0x%x\n", c.x29, c.sp, c.pc );
    printf("x19 addr: 0x%p\n",  &p->cpu_context.x19);

}
