#include "exception.h"
#include "lib.h"
#include "syscall.h"
#include "aarch64.h"
#include "timer.h"
#include "board.h"
#include "gic_v3.h"
#include "printf.h"

void irq_dispatch()
{
    int irq;

//    disable_irq();                    //一进中断。默认关中断
    if( gic_v3_find_pending_irq(&irq) == IRQ_NOT_FOUND ){
        printf("IRQ not found!\n");
        goto restore_irq_out;
    }

//    gicd_disable_int(irq);          /* Mask this irq */
 //   gic_v3_eoi(irq);                /* Send EOI for this irq line */

    switch(irq){
        case TIMER_IRQ:
            timer_handler();
            break;
        default:
            break;
    }

  //  gicd_enable_int(irq);           /* unmask this irq line */

restore_irq_out:

//如果前面定时器已经打开，定时器开始走，此时打开中断，有可能中断还未退出，
//再次进入中断，在第二次进入的中断中，又在此处嵌套第三个中断。。。
//造成栈溢出
//    enable_irq();
    return;
}


void put_exception(unsigned long no, unsigned long lr, unsigned int esr)
{
    unsigned int EC , ISS ;
    
    EC = (esr & ESR_EC_MASK) >> ESR_EC_SHIFT;
    ISS = esr & ESR_ISS_MASK;

    printf("enter exception %d, entry is 0x%x, ", no, lr);
    printf("exception class %#x, ISS is %#x\n", EC, ISS);

    while(1);
}

