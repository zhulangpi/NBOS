#include "syscall.h"




void send_syscall(int syscall_no)
{

    asm("svc    #0x01");

}


void syscall_dispatch(int syscall_no)
{

    switch(syscall_no){
        case 1:
            break;

        default:
            break;
    }


}

