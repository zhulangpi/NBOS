#include "sys_user.h"
#include "lib_user.h"

void main(void)
{
    int i = 3;

    while(1){
        call_sys_write("user process2!\n");
        delay();
/*
        if(i==0){
            call_sys_fork();
            call_sys_write("process2 exit\n");
            call_sys_exit();
        }
        i--;
*/
    }
}


