#include "syscall.h"
#include "printf.h"
#include "task.h"
#include "aarch64.h"
#include "board.h"
#include "fs.h"
#include "lib.h"

void* syscall_table[SYSCALL_NR] = { sys_write, sys_fork, sys_malloc, sys_exit ,sys_open};

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

int sys_open(const char * filename, int flag, int mode)
{

    return file_open(filename, flag, mode);
}

int sys_read(int fd, char * buf, int count)
{
    struct file *filp;

    if(fd>=NR_OPEN || fd<0 || buf==NULL || count<0)
        return 0;
    filp = current->filp[fd];
    if(filp==NULL)
        return 0;
    return file_read(filp, buf, count);
}

