#ifndef _SYS_USER_H
#define _SYS_USER_H

extern int call_sys_write(char *s);
extern int call_sys_fork(void);
extern int call_sys_malloc(unsigned long size);
extern int call_sys_exit(void);

#endif
