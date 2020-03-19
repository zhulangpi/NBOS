#ifndef _SYS_USER_H
#define _SYS_USER_H

#define SEEK_SET    (0)
#define SEEK_CUR    (1)
#define SEEK_END    (2)

extern int call_sys_put(char *s);
extern int call_sys_fork(void);
extern int call_sys_malloc(unsigned long size);
extern int call_sys_exit(void);
extern int call_sys_open(const char * filename, int flag, int mode);
extern int call_sys_read(int fd, char * buf, int count);
extern int call_sys_write(int fd, char * buf, int count);
extern int call_sys_close(int fd);
extern int call_sys_lseek(int fd, int offset, int whence);
extern int call_sys_execv(char *filename);

#endif
