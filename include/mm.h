#ifndef _MM_H_
#define _MM_H_

#define PAGE_SIZE 4096
#define PAGE_SIZE_SHIFT 12
#define LOW_MEM 0x40100000						// 内存低端(1MB)
#define PAGING_MEMORY (15*1024*1024)			// 分页内存15MB，主内存区最多15MB
#define HIGH_MEM 0x40ffffff						// 内存高端(16MB)
#define PAGING_PAGES (PAGING_MEMORY>>PAGE_SIZE_SHIFT)		// 分页后的物理内存页面数(3840)
#define MAP_NR(addr) (((addr)-LOW_MEM)>>PAGE_SIZE_SHIFT)		// 指定内存地址映射的页号
#define USED 100

extern void init_mm();
extern void* kmalloc(unsigned long size);

unsigned long get_free_page();
void free_page(unsigned long p);

#endif
