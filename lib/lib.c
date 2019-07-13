#include "lib.h"
#include "type.h"
#include "board.h"

const char num_ASIIC[16]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };

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

int putc(char c)
{
    UART_DATA = c;
    return 0;
}

int puts(const char *str)
{
    while (*str)
        UART_DATA = *str++;
    return 0;
}

int putlu(unsigned long lu)
{
    int buf[sizeof(unsigned long)*8]={0};
    int idx = 0;
    
    if(!lu)
        putc('0');

    while( lu ){
        buf[idx++] = lu%10;
        lu /= 10;
    }

    while(--idx>=0){
         putc(num_ASIIC[ buf[idx] ]);
    }

    return 0;
}

int putlx(unsigned long lx)
{
    int buf[sizeof(unsigned long)*8/4]={0};
    int idx = 0;
    
    puts("0x");
    if(!lx)
        putc('0');

    while( lx ){
        buf[idx++] = lx%16;
        lx /= 16; 
    }   

    while(--idx>=0){
         putc(num_ASIIC[ buf[idx] ]); 
    }   


    return 0;
}


