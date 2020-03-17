.set SYS_WRITE,     0        // syscal numbers 
.set SYS_FORK,      1
.set SYS_MALLOC,    2
.set SYS_EXIT,      3
.set SYS_OPEN,      4
.set SYS_READ,      5


    .globl  call_sys_write
    .align  2
call_sys_write:
    mov x8, #SYS_WRITE
    svc #0
    ret

    .globl  call_sys_fork
call_sys_fork:
    mov x8, #SYS_FORK
    svc #0
    ret

    .globl  call_sys_malloc
call_sys_malloc:
    mov x8, #SYS_MALLOC
    svc #0
    ret

    .globl  call_sys_exit
call_sys_exit:
    mov x8, #SYS_EXIT
    svc #0
    ret

    .globl  call_sys_open
call_sys_open:
    mov x8, #SYS_OPEN
    svc #0
    ret

    .globl  call_sys_read
call_sys_read:
    mov x8, #SYS_READ
    svc #0
    ret

