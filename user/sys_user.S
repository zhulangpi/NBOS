
.set SYS_WRITE,     0        // syscal numbers 
.set SYS_FORK,      1
.set SYS_MALLOC,    2
.set SYS_EXIT,      3


    .globl  call_sys_write
    .align  2
call_sys_write:
    mov x8, #SYS_WRITE
    svc #0
    br  x30 

    .globl  call_sys_fork
call_sys_fork:
    mov x8, #SYS_FORK
    svc #0
    ret

