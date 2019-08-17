#ifndef _BARRIER_H
#define _BARRIER_H

#define barrier()   __asm__ __volatile__("": : :"memory")

#define mb()        asm volatile("dsb sy": : :"memory")
#define wmb()       asm volatile("dsb st": : :"memory")
#define rmb()       asm volatile("dsb ld": : :"memory")

#define ACCESS_ONCE(x)  (*(volatile typeof(x) *)&(x))

#endif
