#ifndef _SYSCALL_H_
#define _SYSCALL_H_



#define     SYS_PUT     0
#define     SYS_FORK    1
#define     SYS_MALLOC  2
#define     SYS_EXIT    3
#define     SYS_OPEN    4
#define     SYS_READ    5
#define     SYS_WRITE   6
#define     SYS_CLOSE   7
#define     SYS_LSEEK   8
#define     SYSCALL_NR  9


#ifndef _ASSEMBLY_

extern void* syscall_table[];
extern int sys_put(char *s);
extern int sys_fork(void);
extern int sys_malloc(void);
extern int sys_exit(void);
extern int sys_open(const char * filename, int flag, int mode);
extern int sys_read(int fd, char * buf, int count);
extern int sys_write(int fd, char * buf, int count);
extern int sys_close(int fd);
extern int sys_lseek(int fd, int offset, int whence);
#endif


#endif
