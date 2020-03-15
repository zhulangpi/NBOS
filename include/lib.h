#ifndef _LIB_H_
#define _LIB_H_

#define kfree(x) free_s((x), 0)

#ifndef ZLP
#include "printf.h"
#define ZLP
#define zlp_log() printf("%s:%s:%d\n",__FILE__, __func__, __LINE__)
#endif


extern int strcmp(const char * src, const char * dst);
extern unsigned int strlen(const char*str);
extern char * strcpy(char * dst, const char * src);
extern void *memset(void *s, int c, unsigned long n);
extern void memcpy(void *dst, void *src, unsigned long size);
extern void putc(void* p, char c);
extern void _putchar(char c);
extern void *kmalloc(unsigned int len);
extern void free_s(void *obj, int size);
extern void log(void);
extern void kdelay(void);
#endif
