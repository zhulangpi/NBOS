#include "aarch64.h"
#include "board.h"
#include "gic_v3.h"
#include "lib.h"
#include "timer.h"
#include "task.h"
#include "soft_timer.h"
#include "printf.h"

#define TIMER_PERIOD_MS  10

unsigned long long jiffies = 0;

//a constant
//1 jiffies == ticks cycles, 
//timer interrupt every ticks cycles.
static unsigned long ticks;

/* Assert Timer IRQ after TIMER_PERIOD_MS ms */
void timer_handler(void)
{
    unsigned long current_cnt;

	// Disable the timer
	disable_cntv();
	gicd_clear_pending(TIMER_IRQ);

	// Get value of the current timer
	current_cnt = raw_read_cntvct_el0();
	// Set the interrupt in Current Time + TimerTick
	raw_write_cntv_cval_el0(current_cnt + ticks);

    jiffies++;

	// Enable the timer
	enable_cntv();

    //前面定时器已经开始走，该函数内部会打开中断，如果打开中断前
    //定时器已经超时，则会在开中断时再次进入定时器中断，形成嵌套
    //造成栈溢出
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
	ticks = TIMER_PERIOD_MS * (cntfrq / 1000);  //1000 for ms in s

	// Get value of the current timer
	current_cnt = raw_read_cntvct_el0();

	// Set the interrupt in Current Time + TimerTick
	raw_write_cntv_cval_el0(current_cnt + ticks);

	// Enable the timer
	enable_cntv();
}

