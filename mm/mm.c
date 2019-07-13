#include "mm.h"
#include "type.h"


/* heap 物理地址范围 */
extern signed long __pages_start;
extern unsigned long __pages_end;
const unsigned long pages_start = (unsigned long)&__pages_start;
const unsigned long pages_end = (unsigned long)&__pages_end;



char *pheap = NULL;

void init_mm(void)
{
    pheap = (char*)pages_start;
}


void* kmalloc(unsigned long size)
{
    void *p = pheap;
    pheap += size;

    if( !p || ((unsigned long)pheap > pages_end) )
        return NULL;

    return p;
}



