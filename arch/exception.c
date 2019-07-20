#include "exception.h"
#include "lib.h"
#include "syscall.h"
#include "aarch64.h"
#include "timer.h"
#include "board.h"
#include "gic_v3.h"


void irq_dispatch()
{
    int irq;

    disable_irq();
    if( gic_v3_find_pending_irq(&irq) == IRQ_NOT_FOUND ){
        puts("IRQ not found!\n");
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

void exception_dispatch(unsigned int esr_el1)
{
    unsigned int ec = esr_el1&ESR_EC_MASK;// iss = esr_el1&ESR_ISS_MASK;
    
    if(ec == ESR_EC_SVC){   //系统调用
        int syscall_no = esr_el1&ESR_SVC_IMM16_MASK;
        syscall_dispatch(syscall_no);
    }

}


void put_exception(unsigned long no)
{
    puts("enter exception ");
    putlu(no);
    puts("\n");
    while(1);
}

