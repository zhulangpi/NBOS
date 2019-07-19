#include "mm.h"
#include "lib.h"

struct bucket_desc {	/* 32 bytes */
	void			*page;
	struct bucket_desc	*next;
	void			*freeptr;
	unsigned short		refcnt;
	unsigned short		bucket_size;
};

struct _bucket_dir {	/* 16 bytes */
	int			size;
	struct bucket_desc	*chain;
};

struct _bucket_dir bucket_dir[] = {
	{ 16,	(struct bucket_desc *) 0},
	{ 32,	(struct bucket_desc *) 0},
	{ 64,	(struct bucket_desc *) 0},
	{ 128,	(struct bucket_desc *) 0},
	{ 256,	(struct bucket_desc *) 0},
	{ 512,	(struct bucket_desc *) 0},
	{ 1024,	(struct bucket_desc *) 0},
	{ 2048, (struct bucket_desc *) 0},
	{ 4096, (struct bucket_desc *) 0},
	{ 0,    (struct bucket_desc *) 0}};   /* End of list marker */
	
struct bucket_desc *free_bucket_desc = (struct bucket_desc *) 0;

static inline void init_bucket_desc()
{
	struct bucket_desc *bdesc, *first;
	int	i;
	
	first = bdesc = (struct bucket_desc *) get_free_page();
	if (!bdesc)
		puts("Out of memory in init_bucket_desc()\n");
	for (i = PAGE_SIZE/sizeof(struct bucket_desc); i > 1; i--) {
		bdesc->next = bdesc+1;
		bdesc++;
	}
	/*
	 * This is done last, to avoid race conditions in case 
	 * get_free_page() sleeps and this routine gets called again....
	 */
	bdesc->next = free_bucket_desc;
	free_bucket_desc = first;
}

void *kmalloc(unsigned int len)
{
	struct _bucket_dir	*bdir;
	struct bucket_desc	*bdesc;
	void			*retval;

	/*
	 * First we search the bucket_dir to find the right bucket change
	 * for this request.
	 */
	for (bdir = bucket_dir; bdir->size; bdir++)
		if (bdir->size >= len)
			break;

	if (!bdir->size) {
		//printk("malloc called with impossibly large argument (%d)\n",len);
		puts("malloc: bad arg\n");
	}
	/*
	 * Now we search for a bucket descriptor which has free space
	 */
	//cli();	/* Avoid race conditions */
	for (bdesc = bdir->chain; bdesc; bdesc = bdesc->next) 
		if (bdesc->freeptr)
			break;
	/*
	 * If we didn't find a bucket with free space, then we'll 
	 * allocate a new one.
	 */
	if (!bdesc) {
		char		*cp;
		int		i;

		if (!free_bucket_desc)	
			init_bucket_desc();
		bdesc = free_bucket_desc;
		free_bucket_desc = bdesc->next;
		bdesc->refcnt = 0;
		bdesc->bucket_size = bdir->size;
		bdesc->page = bdesc->freeptr = (void *) (cp = (char*)get_free_page());
		if (!cp)
			puts("Out of memory in kernel malloc()\n");
		/* Set up the chain of free objects */
		for (i=PAGE_SIZE/bdir->size; i > 1; i--) {
			*((char **) cp) = cp + bdir->size;
			cp += bdir->size;
		}
		*((char **) cp) = 0;
		bdesc->next = bdir->chain; /* OK, link it in! */
		bdir->chain = bdesc;
	}
	retval = (void *) bdesc->freeptr;
	bdesc->freeptr = *((void **) retval);
	bdesc->refcnt++;
	//sti();	/* OK, we're safe again */
	return(retval);
}

void free_s(void *obj, int size)
{
	void		*page;
	struct _bucket_dir	*bdir;
	struct bucket_desc	*bdesc, *prev;

	/* Calculate what page this object lives in */
	page = (void *)  ((unsigned long) obj & 0xfffff000);
	/* Now search the buckets looking for that page */
	for (bdir = bucket_dir; bdir->size; bdir++) {
		prev = 0;
		/* If size is zero then this conditional is always false */
		if (bdir->size < size)
			continue;
		for (bdesc = bdir->chain; bdesc; bdesc = bdesc->next) {
			if (bdesc->page == page) 
				goto found;
			prev = bdesc;
		}
	}
	puts("Bad address passed to kernel free_s()\n");
found:
	//cli(); /* To avoid race conditions */
	*((void **)obj) = bdesc->freeptr;
	bdesc->freeptr = obj;
	bdesc->refcnt--;
	if (bdesc->refcnt == 0) {
		/*
		 * We need to make sure that prev is still accurate.  It
		 * may not be, if someone rudely interrupted us....
		 */
		if ((prev && (prev->next != bdesc)) ||
		    (!prev && (bdir->chain != bdesc)))
			for (prev = bdir->chain; prev; prev = prev->next)
				if (prev->next == bdesc)
					break;
		if (prev)
			prev->next = bdesc->next;
		else {
			if (bdir->chain != bdesc)
				puts("malloc bucket chains corrupted\n");
			bdir->chain = bdesc->next;
		}
		free_page((unsigned long) bdesc->page);
		bdesc->next = free_bucket_desc;
		free_bucket_desc = bdesc;
	}
	//sti();
	return;
}

void malloc_test(void)
{
	puts("malloc test\n");
	int *p = (int *)kmalloc(sizeof(int));
	int *p1 = (int *)kmalloc(sizeof(int));
	putlx((long)p);puts("\n");
	putlx((long)p1);puts("\n");
	free(p);
	free(p1);

	p = (int *)kmalloc(sizeof(int));
	putlx((long)p);puts("\n");
	free(p);

	p = (int *)kmalloc(4096);
	putlx((long)p);puts("\n");
	free(p);

	p = (int *)kmalloc(4096);
	putlx((long)p);puts("\n");
	free(p);
}