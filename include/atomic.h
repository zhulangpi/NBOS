#ifndef _ATOMIC_H
#define _ATOMIC_H

typedef struct{
    volatile int counter;
}atomic_t;


extern void atomic_set(atomic_t* v, int i);
extern int atomic_read(atomic_t* v);
extern void atomic_add(int i, atomic_t* v);

#endif
