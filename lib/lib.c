#include "lib.h"
#include "type.h"
#include "board.h"

void *memset(void *s, int c, unsigned long n)
{
    if (NULL == s || n < 0)
        return NULL;
    char * tmpS = (char *)s;
    while(n-- > 0)
        *tmpS++ = c;
        return s; 
}

long pow(long x, unsigned long m)
{
    long y=1;
    while(m--){
        y *= x;
    }

    return y;
}

void putc(void *p,  char c)
{
    UART_DATA = c;
}

void log(void)
{
    putc(NULL,'#');
    putc(NULL,'#');
    putc(NULL,'#');
    putc(NULL,'#');
    putc(NULL,'#');
    putc(NULL,'\n');
}
