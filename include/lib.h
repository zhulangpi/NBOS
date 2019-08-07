#ifndef _LIB_H_
#define _LIB_H_

#define kfree(x) free_s((x), 0)

extern void *memset(void *s, int c, unsigned long n);
extern void memcpy(void *dst, void *src, unsigned long size);
extern void putc(void* p, char c);
extern void _putchar(char c);
extern void *kmalloc(unsigned int len);
extern void free_s(void *obj, int size);
extern void log(void);
extern void kdelay(void);
#endif
