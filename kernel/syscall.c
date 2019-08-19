#include "syscall.h"
#include "printf.h"
#include "task.h"
#include "aarch64.h"
#include "board.h"

void* syscall_table[SYSCALL_NR] = { sys_write, sys_fork, sys_malloc, sys_exit };

int sys_write(char *buf)
{
    return printf("%s",buf);
}

int sys_fork(void)
{
    copy_process(USER_PROCESS, PFLASH1_BASE, 16<<10);
    return 0;
}

int sys_malloc(void)
{
    return 0;
}

int sys_exit(void)
{
    current->state = TASK_ZOMBIE;
    schedule();
    return 0;
}


