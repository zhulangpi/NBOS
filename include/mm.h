#ifndef _MM_H_
#define _MM_H_

#ifndef _ASSEMBLY_
#include "type.h"
#endif

#define PAGE_SHIFT              12
#define PAGE_SIZE               (1 << PAGE_SHIFT)
#define TABLE_SHIFT             9
#define SECTION_SHIFT           (PAGE_SHIFT + TABLE_SHIFT)
#define SECTION_SIZE            (1 << SECTION_SHIFT)    
#define MEM_BASE                (0x40000000)
#define LOW_MEM                 (MEM_BASE + (1<<20) )	    // 内存低端(1MB)
#define PAGING_MEMORY           (15 << 20)			    // 分页内存15MB，主内存区最多15MB
#define HIGH_MEM                (MEM_BASE + PAGING_MEMORY - 1)			// 内存高端(16MB)
#define PAGING_PAGES            (PAGING_MEMORY >> PAGE_SHIFT)		// 分页后的物理内存页面数(3840)
#define MAP_NR(addr)            (((addr)-LOW_MEM) >> PAGE_SHIFT)		// 指定内存地址映射的页号
#define USED 100


#define KERNEL_VA_START      (0xffff000000000000)
#define PGD_SHIFT           (39)
#define PUD_SHIFT           (30)
#define PMD_SHIFT           (21)
#define PTE_SHIFT           (12)
#define TABLE_MASK          ((1<<TABLE_SHIFT)-1)
#define SECTION_MASK        ((1<<SECTION_SHIFT)-1)


#ifndef _ASSEMBLY_
struct page {
	atomic_t _count;	/* Usage count */
};

extern unsigned long get_free_page(void);
extern void free_page(unsigned long p); 
#endif

#endif
