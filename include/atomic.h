#ifndef _ATOMIC_H
#define _ATOMIC_H

#include "barrier.h"

typedef struct{
    volatile int counter;
}atomic_t;


#define atomic_set(v,i)     (ACCESS_ONCE((v)->counter)=i)
#define atomic_read(v)      (ACCESS_ONCE((v)->counter))

static inline void atomic_add(int i, atomic_t *v)
{
    unsigned long tmp;
    int result;

    asm volatile(
    "   prfm    pstl1strm, %2\n"    //prefetch
    "1: ldxr    %w0, %2\n"          //w0 = v->counter
    "   add     %w0, %w0, %w3\n"    //w0 += i
    "   stxr    %w1, %w0, %2\n"     //v->counter = w0
    "   cbnz    %w1, 1b"
    : "=&r" (result), "=&r" (tmp), "+Q" (v->counter)    
    : "Ir" (i));
}       

#endif
