#include "mm.h"



/* heap 物理地址范围 */
unsigned long heap_start = 0;
unsigned long heap_end = 0;


void init_mm()
{
    extern unsigned long __heap_start;
    extern unsigned long __heap_end;

    head_start = (unsigned long)&__heap_start;
    head_end = (unsigned long)&__heap_end;


}

