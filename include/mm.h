#ifndef _MM_H_
#define _MM_H_

#define PAGE_SHIFT      12
#define TABLE_SHIFT         9
#define SECTION_SHIFT       (PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE           (1 << PAGE_SHIFT)   
#define SECTION_SIZE        (1 << SECTION_SHIFT)    


#define MEM_BASE            (0x40000000)

#define LOW_MEMORY              (1<<20)  //内核使用的内存，1MB

#define HIGH_MEMORY             (16<<20)    //系统最大内存，16MB

#define PAGING_MEMORY           (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES            (PAGING_MEMORY >> PAGE_SHIFT)


extern unsigned long get_free_page();
extern void free_page(unsigned long p); 
extern unsigned long kmalloc(unsigned long size);
#endif
