#ifndef _MM_H_
#define _MM_H_

#ifndef _ASSEMBLY_
#include "type.h"
#include "atomic.h"
#include "printf.h"
#endif


#define MEM_BASE                (0x40000000)                    // DRAM BASE
#define MEM_SIZE                (1 << 30)                       // DRAM SIZE
#define IMAGE_START             (MEM_BASE + 0x80000)            // QEMU自动将内核镜像搬到该位置，前面的空就不要了
#define LOW_MEM                 (IMAGE_START + (1<<20) )        // 内存低端(1MB)，该部分用于内核镜像
#define PAGING_MEMORY           (15 << 20)                      // 分页内存15MB，页管理器管理的部分
#define HIGH_MEM                (IMAGE_START + PAGING_MEMORY - 1)           // 内存高端(16MB)，超过该地址就越界了


#define PAGE_SHIFT              (12)
#define PAGE_SIZE               (1 << PAGE_SHIFT)
#define TABLE_SHIFT             (9)
#define SECTION_SHIFT           (PAGE_SHIFT + TABLE_SHIFT)
#define SECTION_SIZE            (1 << SECTION_SHIFT)    
#define PAGING_PAGES            (PAGING_MEMORY >> PAGE_SHIFT)
#define PFN_OFFSET              (LOW_MEM >> PAGE_SHIFT)


#define KERNEL_VA_START         (0xffff000000000000)
#define PGD_SHIFT               (39)
#define PUD_SHIFT               (30)
#define PMD_SHIFT               (21)
#define PTE_SHIFT               (12)
#define TABLE_MASK              ((1<<TABLE_SHIFT)-1)
#define SECTION_MASK            ((1<<SECTION_SHIFT)-1)


#define GFP_KERNEL              (0)
#define GFP_USER                (1)


#ifndef _ASSEMBLY_
struct page {
	atomic_t count;
    unsigned long vaddr;
};
extern struct page mem_map[PAGING_PAGES];
#endif

#define pa_to_pfn(pa)           (((pa)-LOW_MEM) >> PAGE_SHIFT)
#define pfn_to_pa(pfn)          ((pfn << PAGE_SHIFT) + LOW_MEM)
#define page_to_pfn(page)       ((unsigned long)((page) - mem_map) + PFN_OFFSET)
#define pfn_to_page(pfn)        ( mem_map + ((pfn) - PFN_OFFSET))
#define pa_to_page(pa)          (pfn_to_page(pa_to_pfn(pa)))
#define page_to_pa(page)        (pfn_to_pa(page_to_pfn(page)))


#define __va(pa)                ((void*)( pa - MEM_BASE + KERNEL_VA_START))
#define __pa(va)                ((unsigned long)va - KERNEL_VA_START + MEM_BASE)


#ifndef _ASSEMBLY_


struct mm_struct{



};


extern struct page* alloc_page(void);
extern void* get_free_page(int gfp_flags);
extern void __free_page(struct page*);
extern void free_page(unsigned long addr);
#endif

#endif
