#include "syscall.h"
#include "printf.h"
#include "task.h"



void* syscall_table[SYSCALL_NR] = { sys_write, sys_fork, sys_malloc, sys_exit };

void sys_write(char *buf)
{
    printf("%s",buf);
}

void sys_fork( void (*main)(void) )
{
    copy_process( USER_PROCESS, main);
}

void sys_malloc(void)
{
}

void sys_exit(void)
{
}


