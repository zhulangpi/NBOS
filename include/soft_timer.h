#ifndef _SOFT_TIMER_H_
#define _SOFT_TIMER_H_

#include "list.h"

//timer mode
#define ONESHOT     0
#define PERIODIC    1


struct timer{
    struct list_head list;
    unsigned long count;    //unit is jiffies
    unsigned long long goal;
    void (*main)(void);
    unsigned long mode;
};

extern void timer_add( unsigned long count, void (*main)(void), unsigned long mode );
extern void timer_delete(struct timer* t);
extern void walk_timer_list(void);
extern void init_timer(void);

#endif
