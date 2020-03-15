#include "lib.h"
#include "type.h"
#include "board.h"

//字典序比较两字符串大小
int strcmp(const char * src, const char * dst)
{
    int ret = 0 ;
    while((!(ret=*src-*dst))&&*dst)   //相等且没有结束
        ++src, ++dst;
    return( ret );
}

unsigned int strlen(const char*str)
{
    unsigned int len=0;
    while((*str++)!='\0') 
        len++;
    return len;
}

char * strcpy(char * dst, const char * src)
{
    char *dst_cpy;

    if ((NULL==dst) || (NULL==src)) 
        return NULL;

    dst_cpy = dst; 
    while ((*dst++=*src++)!='\0'); 
    return dst_cpy;
}

void *memset(void *s, int c, unsigned long n)
{
    if (NULL == s || n < 0)
        return NULL;
    char * tmpS = (char *)s;
    while(n-- > 0)
        *tmpS++ = c;
        return s; 
}

void memcpy(void *dst, void* src, unsigned long size)
{
    int i = 0;
    for(i=0;i<size;i++){
        *((char*)dst+i) = *((char*)src+i);
    }
}

long pow(long x, unsigned long m)
{
    long y=1;
    while(m--){
        y *= x;
    }

    return y;
}

void putc(void* p,char c)
{
    UART_DATA = c;
}

void _putchar(char c)
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


void kdelay(void)
{
    unsigned int i = 1<<24;
    while(i--){
    }   
}

