#include "mmu.h"
#include "mm.h"
#include "type.h"
#include "lib.h"
#include "printf.h"
#include "atomic.h"
#include "board.h"

extern unsigned long __img_end;
const void* img_end = &__img_end;

void init_mm(void)
{
	map_kernel_sections( PFLASH1_PA_BASE, PFLASH1_BASE, PFLASH1_SIZE,  MMU_FLAGS );

    if(__pa(img_end) > HIGH_MEM){
        printf("image is too large\n");
    }
}

struct page mem_map[PAGING_PAGES] = {0};


//分配一页
static struct page* alloc_page(void)
{
	int i = PAGING_PAGES - 1;
	for(; i >= 0; i--){
		if( atomic_read( &( (mem_map+i)->count) ) == 0){
			atomic_set( &( (mem_map+i)->count ), 1 );
			return mem_map+i;
		}
	}
    printf("alloc_page failed, there is no page in mem_map[]\n");
	return NULL;
}

void __free_page(struct page* p)
{
    atomic_add( -1, &p->count );
    if(atomic_read( &p->count )==0){
        ;//释放内存
    }
}

void free_page(void* addr)
{
    __free_page( pa_to_page(__pa(addr)) );
}

// 分配一页并获得其在内核空间的VA
void* get_free_page(int gfp_flags)
{
    struct page* page = alloc_page();
    (void)gfp_flags;
    memset(__va(page_to_pa(page)) , 0, PAGE_SIZE );
    return  __va(page_to_pa(page));
}


static void map_kernel_section( unsigned long pa, unsigned long addr, unsigned long flags )
{
    unsigned long pgd_index = 0, pud_index = 0, pmd_index = 0;
    unsigned long *pgd_entry = NULL, *pud_entry = NULL, *pmd_entry = NULL;
    void* va;

	extern const unsigned long pg_tbl_start;
	unsigned long pgd = (unsigned long)&pg_tbl_start;


	if(flags!=MMU_FLAGS && flags!=MMU_DEVICE_FLAGS){
		printf("flags error, map failed\n");
		return;
	}

    pgd_index = (addr >> PGD_SHIFT) & TABLE_MASK;
    pud_index = (addr >> PUD_SHIFT) & TABLE_MASK;
    pmd_index = (addr >> PMD_SHIFT) & TABLE_MASK;

    //计算PGD表项地址
    pgd_entry = (void*)pgd + (pgd_index << ENTRY_SHIFT);
    if(*pgd_entry==0){  //分配PUD页，填充PGD表项
        va = get_free_page(GFP_KERNEL);
        *pgd_entry = __pa(va) | MM_TYPE_PAGE_TABLE;
    }   

    pud_entry = __va(ENTRY_GET_ADDR(*pgd_entry)) + (pud_index << ENTRY_SHIFT);
    if(*pud_entry==0){
        va = get_free_page(GFP_KERNEL);
        *pud_entry = __pa(va) | MM_TYPE_PAGE_TABLE;
    }   

    pmd_entry = __va(ENTRY_GET_ADDR(*pud_entry)) + (pmd_index << ENTRY_SHIFT);
    if(*pmd_entry==0){
        *pmd_entry = (pa & (~SECTION_MASK)) | flags;
    }   

}

void map_kernel_sections(unsigned long pa, unsigned long addr, unsigned long size, unsigned long flags)
{
    unsigned long sz = (size & (SECTION_SIZE-1))?( (size >> SECTION_SHIFT) + 1):(size >> SECTION_SHIFT);
    int i = 0;
    for(i = 0; i < sz; i++){
        map_kernel_section( pa + (i << SECTION_SHIFT), addr + (i << SECTION_SHIFT), flags );
    }
}


static void alloc_user_page( unsigned long addr, struct mm_struct* mm )
{
    unsigned long pgd_index = 0, pud_index = 0, pmd_index = 0, pte_index = 0;
    unsigned long *pgd_entry = NULL, *pud_entry = NULL, *pmd_entry = NULL, *pte_entry = NULL;
    int i = 0, idx = 0;
    void* va;

    pgd_index = (addr >> PGD_SHIFT) & TABLE_MASK;
    pud_index = (addr >> PUD_SHIFT) & TABLE_MASK;
    pmd_index = (addr >> PMD_SHIFT) & TABLE_MASK;
    pte_index = (addr >> PTE_SHIFT) & TABLE_MASK;

    for(idx=0; idx<MAX_PGTBL_PAGES; idx++){
        if( mm->pgtbl_page[idx]==NULL ){
            break;
        }
    }

    //计算PGD表项地址
    pgd_entry = __va(mm->pgd) + (pgd_index << ENTRY_SHIFT);
    if(*pgd_entry==0){  //分配PUD页，填充PGD表项
        va = get_free_page(GFP_KERNEL);
        *pgd_entry = __pa(va) | MM_TYPE_PAGE_TABLE;
        mm->pgtbl_page[idx++] = va;
    }

    pud_entry = __va(ENTRY_GET_ADDR(*pgd_entry)) + (pud_index << ENTRY_SHIFT);
    if(*pud_entry==0){
        va = get_free_page(GFP_KERNEL);
        *pud_entry = __pa(va) | MM_TYPE_PAGE_TABLE;
        mm->pgtbl_page[idx++] = va;
    }

    pmd_entry = __va(ENTRY_GET_ADDR(*pud_entry)) + (pmd_index << ENTRY_SHIFT);
    if(*pmd_entry==0){
        va = get_free_page(GFP_KERNEL);
        *pmd_entry = __pa(va) | MM_TYPE_PAGE_TABLE;
        mm->pgtbl_page[idx++] = va;
    }

    pte_entry = __va(ENTRY_GET_ADDR(*pmd_entry)) + (pte_index << ENTRY_SHIFT);
    if(*pte_entry==0){
        va = get_free_page(GFP_KERNEL);
        *pte_entry = __pa(va) | MMU_PTE_FLAGS;
    }

    for(i = 0; i < MAX_USER_PAGES; i++){
        if(mm->vma[i].pa==0){
            mm->vma[i].va = addr;
            mm->vma[i].pa = ENTRY_GET_ADDR(*pte_entry);
            break;
        }
    }
}


//给任务p分配并建立用户态[start, start+size]的映射
void alloc_user_pages( unsigned long start, unsigned long size, struct mm_struct* mm )
{
    //对size向上取整，共映射sz个页面
    unsigned long sz = (size & (PAGE_SIZE-1))?( (size >> PAGE_SHIFT) + 1):(size >> PAGE_SHIFT);
    int i = 0;
    for(i = 0; i < sz; i++){
        alloc_user_page( start + (i << PAGE_SHIFT), mm);
    }
}


void print_process_page(struct mm_struct *mm)
{
    int i = 0;
    if(!mm)
        return ;
    printf("user page:\n");
    for(i=0;i<MAX_USER_PAGES;i++){
        if(mm->vma[i].pa!=0){
            printf("va: 0x%p,\t pa: 0x%p\n", mm->vma[i].va, mm->vma[i].pa);
        }
    }
    printf("pgtbl page:\n");
    for(i=0;i<MAX_PGTBL_PAGES;i++){
        if(mm->pgtbl_page[i]!=NULL){
            printf("va: 0x%p,\t pa: 0x%p\n", mm->pgtbl_page[i], __pa(mm->pgtbl_page[i]) );
        }
    }
}


