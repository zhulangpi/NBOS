#ifndef _SYSCALL_H_
#define _SYSCALL_H_


#define     SYS_WRITE   0
#define     SYS_FORK    1
#define     SYS_MALLOC  2
#define     SYS_EXIT    3
#define     SYSCALL_NR  4

#ifndef _ASSEMBLY_

extern void* syscall_table[];
extern void sys_write();
extern void sys_fork();
extern void sys_malloc();
extern void sys_exit();

#endif


#endif
