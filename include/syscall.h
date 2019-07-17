#ifndef _SYSCALL_H_
#define _SYSCALL_H_


extern void send_syscall(int syscall_no);
extern void syscall_dispatch(int syscall_no);



#endif
