#ifndef _MM_H_
#define _MM_H_

#ifndef _ASSEMBLY_
#include "type.h"
#include "atomic.h"
#include "printf.h"
#include "fs.h"
#endif


#define MEM_BASE                (0x40000000)                    // DRAM BASE
#define MEM_SIZE                (1 << 30)                       // DRAM SIZE
#define IMAGE_START             (MEM_BASE + 0x80000)            // QEMU自动将内核镜像搬到该位置，前面的空就不要了
#define LOW_MEM                 (IMAGE_START + (1<<20) )        // 内存低端(1MB)，该部分用于内核镜像
#define PAGING_MEMORY           (15 << 20)                      // 分页内存15MB，页管理器管理的部分
#define HIGH_MEM                (IMAGE_START + PAGING_MEMORY - 1)           // 内存高端(16MB)，超过该地址就越界了


#define PAGE_SHIFT              (12)
#define PAGE_SIZE               (1 << PAGE_SHIFT)
#define PAGE_MASK               ((1 << PAGE_SHIFT) - 1)
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
#define TABLE_MASK              ((1 << TABLE_SHIFT) - 1)
#define SECTION_MASK            ((1 << SECTION_SHIFT) - 1)
#define ENTRY_SHIFT             (3)     //one page table entry need 64bit(8B)

//去掉页表项的低位属性位
#define ENTRY_GET_ADDR(m)         (m & (~PAGE_MASK))

#define USER_START              (0)
#define USER_MAX                (1<<30)

#define GFP_KERNEL              (1 << 0)
#define GFP_USER                (1 << 1)


#define PG_free                 (0)         //空闲
#define PG_kmalloc              (1)         //该页被用于kmalloc
//#define PG_buffer_head          (2)         //该页被用作buffer_head管理的cache或buffer

#ifndef _ASSEMBLY_
struct page {
    atomic_t flags;
	atomic_t count;         //引用计数，为1时表示被分配但可以释放
    struct buffer_head *fbh;       //this page's first buffer_head

    struct list_head lru;   //用作page cache时，链接所有的page(暂不实现page cache)
//    unsigned long vaddr;  //计划用于反向映射，暂时未实现
};

extern struct page mem_map[PAGING_PAGES];
#endif

//PFN: 从可分配的分页内存开始，第一页PFN = 0
//所有的物理页都可以通过内核地址来访问，全都已经建立好了映射
#define pa_to_pfn(pa)           (((pa)-LOW_MEM) >> PAGE_SHIFT)
#define pfn_to_pa(pfn)          ((pfn << PAGE_SHIFT) + LOW_MEM)
#define page_to_pfn(page)       ((unsigned long)((page) - mem_map))
#define pfn_to_page(pfn)        ( mem_map + pfn)
#define pa_to_page(pa)          (pfn_to_page(pa_to_pfn(pa)))
#define page_to_pa(page)        (pfn_to_pa(page_to_pfn(page)))

#define __va(pa)                ((void*)( (pa - MEM_BASE) + (unsigned long)KERNEL_VA_START))
#define __pa(va)                ((unsigned long)va - KERNEL_VA_START + MEM_BASE)


#define MAX_USER_PAGES          (16)
#define MAX_PGTBL_PAGES         (8)

#ifndef _ASSEMBLY_

struct vm_area_struct{
    unsigned long va;
    unsigned long pa;
};

struct mm_struct{
    unsigned long pgd;              //页表首地址(PA)
    struct vm_area_struct vma[MAX_USER_PAGES];
    void* pgtbl_page[MAX_PGTBL_PAGES];
};

extern void init_mm(void);
extern void* get_free_page(int gfp_flags);
extern void __free_page(struct page*);
extern void free_page(void* addr);
extern void map_kernel_sections(unsigned long pa, unsigned long addr, unsigned long size,  unsigned long flags);
extern void alloc_user_pages( unsigned long start, unsigned long size, struct mm_struct* mm );
extern void print_process_page(struct mm_struct *mm);
#endif

#endif
