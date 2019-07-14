#include "mm.h"
#include "type.h"
#include "lib.h"


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

// 物理内存映射字节图(1字节代表1页内存)。每个页面对应的字节用于标志页面当前被引用
// (占用)次数。它最大可以映射15MB的内存空间。在初始化函数mem_init()中，对于不能用
// 做主内存区页面的位置均都预先被设置成USED(100)
static unsigned char mem_map [ PAGING_PAGES ] = {0,};

// 在主内存区中取空闲物理页面。如果已经没有可用物理内存页面，则返回0
// 本函数从位图末端开始向前扫描所有页面标志(页面总数为PAGING_PAGES)
// 若有页面空闲(内存位图字节为0)则返回页面地址
unsigned long get_free_page(void)
{
	int i = PAGING_PAGES - 1;
	for(; i >= 0; i--)
	{
		if(mem_map[i] == 0)
		{
			mem_map[i] = 1;
			unsigned long page = LOW_MEM + i*PAGE_SIZE;
			memset((void *)page, 0, PAGE_SIZE);
			return page;	//返回实际物理地址			
		}
	}
	return 0;
}

// 释放物理地址addr开始的1页页面内存
// 物理地址1MB以下的内存空间用于内核程序和缓冲，不作为分配页面的额内存空间。因此
// 参数addr需要大于1MB
void free_page(unsigned long addr)
{
	if(addr < LOW_MEM || addr > HIGH_MEM)
		puts("trying to free nonexistent page\n");
	mem_map[(addr - LOW_MEM) >> PAGE_SIZE_SHIFT] = 0;//未作异常地址判断
}
