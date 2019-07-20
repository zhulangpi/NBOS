#include "mm.h"
#include "type.h"


/* heap 物理地址范围 */
extern signed long __pages_start;
extern unsigned long __pages_end;
const unsigned long pages_start = (unsigned long)&__pages_start;
const unsigned long pages_end = (unsigned long)&__pages_end;


static unsigned short mem_map [ PAGING_PAGES ] = {0,};

unsigned long kmalloc(unsigned long size)
{
    return 0;
}

unsigned long get_free_page()
{
    for (int i = 0; i < PAGING_PAGES; i++){
        if (mem_map[i] == 0){
            mem_map[i] = 1;
            return MEM_BASE + LOW_MEMORY + i*PAGE_SIZE;
        }
    }
    return 0;
}

void free_page(unsigned long p){
    mem_map[p>>PAGE_SHIFT] = 0;
}

