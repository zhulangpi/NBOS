#include "type.h"
#include "mm.h"
#include "timer.h"
#include "soft_timer.h"
#include "lib.h"
#include "task.h"
#include "list.h"

//timer list head
LIST_HEAD(timer_head);


void timer_add( unsigned long count, void (*main)(void), unsigned long mode )
{
    struct timer *new;
    new = (struct timer*)kmalloc(sizeof(struct timer));

    new->count = count;
    new->goal = jiffies + count;
    new->main = main;
    new->mode = mode;

    list_add(&new->list, &timer_head);
}


void timer_delete(struct timer* t)
{
    list_del(&t->list);
    kfree(t);
}


void walk_timer_list(void)
{
    struct list_head *pos;
    struct timer *tmp;

    list_for_each(pos, &timer_head){
        tmp = list_entry(pos, struct timer, list);
        
        if(tmp->goal<jiffies)
            continue;

        if(tmp->main)
            tmp->main();
        if(tmp->mode == PERIODIC){
            tmp->goal = jiffies + tmp->count;
        }else{
            timer_delete(tmp);
        }
    }
}

void timer_handler0()
{
    printf("this is timer_handler\n");
}

void init_timer(void)
{
    timer_add(100, timer_handler0, PERIODIC);
}


