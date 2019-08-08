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

    disable_irq();
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
    enable_irq();
    return;
}


void put_exception(unsigned long no, unsigned long lr)
{
    printf("enter exception %d, entry is 0x%x\n", no, lr);
    while(1);
}

