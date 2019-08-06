#include "sys_user.h"
#include "lib_user.h"

void main(void)
{
    while(1){
        call_sys_write("user process1!\n");
        delay();
    }

}




