#include "syscall.h"
#include "printf.h"
#include "task.h"
#include "aarch64.h"
#include "board.h"
#include "fs.h"
#include "lib.h"

void* syscall_table[SYSCALL_NR] = { sys_put, sys_fork, sys_malloc, sys_exit, sys_open, sys_read, sys_write, sys_lseek};

int sys_put(char *buf)
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

    if( (fd>=NR_OPEN) || (fd<0) || (buf==NULL) || (count<0) )
        return -1;
    filp = current->filp[fd];
    if(filp==NULL)
        return -1;

    return file_read(filp, buf, count);
}

int sys_write(int fd, char * buf, int count)
{
    struct file *filp;

    if( (fd>=NR_OPEN) || (fd<0) || (buf==NULL) || (count<0) )
        return -1;
    filp = current->filp[fd];
    if(filp==NULL)
        return -1;

    return file_write(filp, buf, count);
}

int sys_lseek(int fd, int offset, int whence)
{
    struct file *filp;
    if( (fd>=NR_OPEN) || (fd<0))
        return -1;
    filp = current->filp[fd];
    if(filp==NULL)
        return -1;

    return file_lseek(filp, offset, whence);
}


