.set SYS_PUT,       0        // syscal numbers 
.set SYS_FORK,      1
.set SYS_MALLOC,    2
.set SYS_EXIT,      3
.set SYS_OPEN,      4
.set SYS_READ,      5
.set SYS_WRITE,     6
.set SYS_CLOSE,     7
.set SYS_LSEEK,     8
.set SYS_EXECV,     9


    .globl  call_sys_put
    .align  2
call_sys_put:
    mov x8, #SYS_PUT
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

    .globl  call_sys_write
call_sys_write:
    mov x8, #SYS_WRITE
    svc #0
    ret

    .globl  call_sys_close
call_sys_close:
    mov x8, #SYS_CLOSE
    svc #0
    ret

    .globl  call_sys_lseek
call_sys_lseek:
    mov x8, #SYS_LSEEK
    svc #0
    ret

    .globl  call_sys_execv
call_sys_execv:
    mov x8, #SYS_EXECV
    svc #0
    ret
