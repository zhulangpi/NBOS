#ifndef _ATOMIC_H
#define _ATOMIC_H
#include "aarch64.h"
#include "type.h"

typedef struct{
    volatile int counter;
}atomic_t;


__always_inline void atomic_set(atomic_t* v, int i)
{
    unsigned long flag;
    disable_irqsave(&flag);
    v->counter = i;
    enable_irqsave(flag);
}

__always_inline int atomic_read(atomic_t* v)
{
    return v->counter;
}

__always_inline void atomic_add(int i, atomic_t* v)
{
    unsigned long flag;
    disable_irqsave(&flag);
    v->counter += i;
    enable_irqsave(flag);
}


#endif
