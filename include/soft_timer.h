#ifndef _SOFT_TIMER_H_
#define _SOFT_TIMER_H_



#define TIMERS_MAX   32



struct timer{
    unsigned long goal_ticks;
    void (*main)(void);
};



void timer_add( unsigned long count, void (*main)(void) );
void timer_delete(struct timer* t);
void walk_timer_list(void);

#endif
