#include "exception.h"
#include "lib.h"
#include "syscall.h"
#include "timer.h"
#include "board.h"
#include "gic_v3.h"



void irq_handle(struct exception_frame *exc)
{
 //   psw_t psw;
    int irq;
    int rc;

//    psw_disable_and_save_interrupt(&psw);
    rc = gic_v3_find_pending_irq(exc, &irq);
    if( rc == IRQ_FOUND ){
        puts("IRQ found: ");
        putlx(irq);
        puts("\n");
    }else{
        puts("IRQ not found!\n");
        goto restore_irq_out;
    }

    gicd_disable_int(irq);          /* Mask this irq */
    gic_v3_eoi(irq);                /* Send EOI for this irq line */
    if(irq==TIMER_IRQ)
        timer_handler();
    gicd_enable_int(irq);           /* unmask this irq line */

restore_irq_out:
//    psw_restore_interrupt(&psw);
    return;
}


void common_trap_handler(struct exception_frame *exc)
{
    puts("Exception Handler! (");
    //handle_exception(exc);

    if ( ( exc->exc_type & 0xff ) == AARCH64_EXC_SYNC_SPX ) { 
        puts("AARCH64_EXC_SYNC_SPX)\n");

        if( ( exc->exc_esr & ESR_EC_MASK ) == ESR_EC_SVC ){
            int syscall_no = exc->exc_esr & ESR_SVC_IMM16_MASK;
            syscall(syscall_no);

        }

 //       handle_exception(exc);
/*
        ti_update_preempt_count(ti, THR_EXCCNT_SHIFT, 1);
        psw_enable_interrupt();
        hal_handle_exception(exc);
        psw_disable_interrupt();
        ti_update_preempt_count(ti, THR_EXCCNT_SHIFT, -1);
*/
    }   

    if ( ( exc->exc_type & 0xff ) == AARCH64_EXC_IRQ_SPX) {
        puts("AARCH64_EXC_IRQ_SPX)\n");
        irq_handle(exc);
    }   
    return;
}





void put_exception(unsigned long idx)
{

    puts("enter el1 exception ");
    
    switch(idx){
        case 0:
            puts("ex0\n");
            break;
        case 1:
            puts("ex1\n");
            break;
        case 2:
            puts("ex2\n");
            break;
        case 3:
            puts("ex3\n");
            break;

        case 4:
            puts("ex4\n");
            break;
        case 5:
            puts("ex5\n");
            break;
        case 6:
            puts("ex6\n");
            break;
        case 7:
            puts("ex7\n");
            break;

        case 8:
            puts("ex8\n");
            break;
        case 9:
            puts("ex9\n");
            break;
        case 10:
            puts("ex10\n");
            break;
        case 11:
            puts("ex11\n");
            break;

        case 12:
            puts("ex12\n");
            break;
        case 13:
            puts("ex13\n");
            break;
        case 14:
            puts("ex14\n");
            break;
        case 15:
            puts("ex15\n");
            break;
    }

    while(1);
}










