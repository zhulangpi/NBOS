#include "sys_user.h"
#include "lib_user.h"

void main(void)
{
//    int i = 3;

    while(1){
        call_sys_put("user process!\n");
        delay();
        


/*
        if(i==0){
            call_sys_fork();
            call_sys_put("process exit\n");
            call_sys_exit();
        }
        i--;
*/
    }
}


