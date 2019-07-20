#include "aarch64.h"
#include "board.h"
#include "gic_v3.h"
#include "lib.h"
#include "timer.h"
#include "task.h"
#include "soft_timer.h"

#define TIMER_PERIOD_MS  1

unsigned long ticks;

/* Assert Timer IRQ after TIMER_PERIOD_MS ms */
void timer_handler(void)
{
    unsigned long current_cnt;

    puts("timer_handler: \n");

	// Disable the timer
	disable_cntv();
	gicd_clear_pending(TIMER_IRQ);

	// Get value of the current timer
	current_cnt = raw_read_cntvct_el0();
	// Set the interrupt in Current Time + TimerTick
	raw_write_cntv_cval_el0(current_cnt + ticks);
	// Enable the timer
	enable_cntv();


    scheduler_tick();
}

void timer_init(void)
{
    unsigned int cntfrq;
	unsigned long current_cnt;

	// GIC Init
	gic_v3_initialize();

	// Disable the timer
	disable_cntv();

	cntfrq = raw_read_cntfrq_el0();

	// Next timer IRQ is after n ms.
	ticks = TIMER_PERIOD_MS * (cntfrq / 1000);

	// Get value of the current timer
	current_cnt = raw_read_cntvct_el0();

	// Set the interrupt in Current Time + TimerTick
	raw_write_cntv_cval_el0(current_cnt + ticks);

	// Enable the timer
	enable_cntv();

	// Enable IRQ 
//	enable_irq();

}

