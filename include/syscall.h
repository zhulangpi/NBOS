#ifndef _SYSCALL_H_
#define _SYSCALL_H_


#define     SYS_WRITE   0
#define     SYS_FORK    1
#define     SYS_MALLOC  2
#define     SYS_EXIT    3
#define     SYSCALL_NR  4

#ifndef _ASSEMBLY_

extern void* syscall_table[];
extern int sys_write(char *s);
extern int sys_fork(void);
extern int sys_malloc(void);
extern int sys_exit(void);

#endif


#endif
