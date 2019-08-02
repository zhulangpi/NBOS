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
    if(gfp_flags==GFP_KERNEL){

       return  __va(page_to_pa(page));

    }else if(gfp_flags==GFP_USER){
    //映射该页到当前进程的地址空间中
        ;
    }

    return 0;
}




