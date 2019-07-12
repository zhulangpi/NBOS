#include "mm.h"
#include "type.h"


/* heap 物理地址范围 */
extern signed long __heap_start;
extern unsigned long __heap_end;
const unsigned long heap_start = (unsigned long)&__heap_start;
const unsigned long heap_end = (unsigned long)&__heap_end;



char *pheap = NULL;

void init_heap(void)
{
    pheap = (char*)heap_start;
}


void* kmalloc(unsigned long size)
{
    void *p = pheap;
    pheap += size;

    if( (unsigned long)pheap > heap_end )
        return NULL;

    return p;
}



