#include "type.h"
#include "mm.h"
#include "timer.h"
#include "soft_timer.h"



static struct timer* timer_list[TIMERS_MAX]={0};


void timer_add( unsigned long count, void (*main)(void) )
{
    struct timer *new = NULL;
    int idx = 0;
    new = (struct timer*)kmalloc(sizeof(struct timer));

    new->goal_ticks = ticks + count;
    new->main = main;

    for( idx=0;idx<TIMERS_MAX;idx++ ){
        if( timer_list[idx] == NULL ){
            timer_list[idx] = new;
        }

    }


}


void timer_delete(struct timer* t)
{
    int idx=0;

    for(idx=0;idx<TIMERS_MAX;idx++){
        if(timer_list[idx]==t){
//            kfree(t);
            timer_list[idx] = NULL;
        }

    }

}


void walk_timer_list(void)
{
    int idx = 0;

    for( idx=0;idx<TIMERS_MAX;idx++ ){
        if( (timer_list[idx]!=NULL)&&(timer_list[idx]->goal_ticks>ticks )){
            timer_list[idx]->main();
            timer_delete(timer_list[idx]);
        }

    }

}



