#ifndef _LIB_H_
#define _LIB_H_

#define free(x) free_s((x), 0)

extern void *memset(void *s, int c, unsigned long n);
extern int putc(char c);
extern int puts(const char *str);
extern int putlu(unsigned long lu);
extern int putlx(unsigned long lx);
extern void *kmalloc(unsigned int len);
extern void free_s(void *obj, int size);
extern void malloc_test(void);

#endif
