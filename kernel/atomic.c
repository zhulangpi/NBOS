#include "aarch64.h"
#include "atomic.h"


void atomic_set(atomic_t* v, int i)
{
    unsigned long flag;
    disable_irqsave(&flag);
    v->counter = i;
    enable_irqsave(flag);
}

int atomic_read(atomic_t* v)
{
    return v->counter;
}

void atomic_add(int i, atomic_t* v)
{
    unsigned long flag;
    disable_irqsave(&flag);
    v->counter += i;
    enable_irqsave(flag);
}

