#ifndef _LIB_H_
#define _LIB_H_

#include "../include/reg.h"
#include "../include/type.h"

extern void *memset(void *s, int c, unsigned long n);
extern int putc(char c);
extern int puts(const char *str);
extern int putlu(unsigned long lu);
extern int putlx(unsigned long lx);

#endif
