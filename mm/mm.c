#include "mmu.h"
#include "mm.h"
#include "type.h"
#include "lib.h"
#include "printf.h"
#include "atomic.h"

extern unsigned long __img_end;
const void* img_end = &__img_end;

void init_mm(void)
{
    if(__pa(img_end) > HIGH_MEM){
        printf("image is too large\n");
    }
}

struct page mem_map[PAGING_PAGES] = {0};


//分配一页
struct page* alloc_page(void)
{
	int i = PAGING_PAGES - 1;
	for(; i >= 0; i--){
		if( atomic_read( &( (mem_map+i)->count) ) == 0){
			atomic_set( &( (mem_map+i)->count ), 1 );
			return mem_map+i;
		}
	}
	return NULL;
}

void __free_page(struct page* p)
{
    atomic_add( -1, &p->count );
    if(atomic_read( &p->count )==0){
        ;//释放内存
    }
}

void free_page(unsigned long addr)
{
    __free_page( pa_to_page(__pa(addr)) );
}

// 分配一页并获得VA
void* get_free_page(int gfp_flags)
{
    struct page* page = alloc_page();
    if(!page){
        printf("alloc page failed\n");
        return NULL;
    }

    if(gfp_flags==GFP_KERNEL){
        memset(__va(page_to_pa(page)) , 0, PAGE_SIZE );
        return  __va(page_to_pa(page));

    }
    printf("get free page failed\n");
    return NULL;
}


static void alloc_user_page( unsigned long addr, struct mm_struct* mm )
{
    unsigned long pgd_index = 0, pud_index = 0, pmd_index = 0, pte_index = 0;
    unsigned long *pgd_entry = NULL, *pud_entry = NULL, *pmd_entry = NULL, *pte_entry = NULL;

    pgd_index = (addr >> PGD_SHIFT) & TABLE_MASK;
    pud_index = (addr >> PUD_SHIFT) & TABLE_MASK;
    pmd_index = (addr >> PMD_SHIFT) & TABLE_MASK;
    pte_index = (addr >> PTE_SHIFT) & TABLE_MASK;

    //计算PGD表项地址
    pgd_entry = __va(mm->pgd) + (pgd_index << ENTRY_SHIFT);
    if(*pgd_entry==0){  //分配PUD页，填充PGD表项
        *pgd_entry = __pa(get_free_page(GFP_KERNEL)) | MM_TYPE_PAGE_TABLE;
    }

    pud_entry = __va(ENTRY_GET_ADDR(*pgd_entry)) + (pud_index << ENTRY_SHIFT);

    if(*pud_entry==0){
        *pud_entry = __pa(get_free_page(GFP_KERNEL)) | MM_TYPE_PAGE_TABLE;
    }

    pmd_entry = __va(ENTRY_GET_ADDR(*pud_entry)) + (pmd_index << ENTRY_SHIFT);
    if(*pmd_entry==0){
        *pmd_entry = __pa(get_free_page(GFP_KERNEL)) | MM_TYPE_PAGE_TABLE;
    }

    pte_entry = __va(ENTRY_GET_ADDR(*pmd_entry)) + (pte_index << ENTRY_SHIFT);
    if(*pte_entry==0){
        *pte_entry = __pa(get_free_page(GFP_KERNEL)) | MMU_PTE_FLAGS;
    }

}


//给任务p分配并建立用户态[start, start+size]的映射
void alloc_user_pages( unsigned long start, unsigned long size, struct mm_struct* mm )
{
    //对size向上取整，共映射sz个页面
    unsigned long sz = (size & (PAGE_SIZE-1))?( (size >> PAGE_SHIFT) + 1):(size >> PAGE_SHIFT);
    struct vm_area_struct *vma = (struct vm_area_struct*)kmalloc(sizeof(struct vm_area_struct));
    int i = 0;
    for(i = 0; i < sz; i++){
        alloc_user_page( start + (i << PAGE_SHIFT), mm);
    }

    vma->start = start;
    vma->size = sz;
    vma->next = NULL;
    mm->vma = vma;
}

