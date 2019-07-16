#include "syscall.h"




void syscall(int syscall_no)
{

    asm("svc    #0x01");

}
