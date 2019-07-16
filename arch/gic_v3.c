/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Arm Generic Interrupt Controller(PL390)                           */
/*                                                                    */
/**********************************************************************/

/**********************************************************************/
/*  armv8-bare-metal                                                  */
/*  Copyright 2018 Nienfeng Yao                                       */
/*                                                                    */
/*  Reference: ARM® Generic Interrupt Controller Architecture         */
/*      Specification GIC architecture version 3.0 and version 4.0    */
/*                                                                    */
/**********************************************************************/
#include "board.h"
#include "exception.h"
#include "gic_v3.h"
#include "lib.h"


/* Initialize GIC Controller */
static void init_gicc(void)
{
	unsigned int pending_irq;

    puts("init_gicc()\n");
	/* Disable CPU interface */
	*REG_GIC_GICC_CTLR = GICC_CTLR_DISABLE;

	/* Set the priority level as the lowest priority */
	/* Note: Higher priority corresponds to a lower Priority field value in the GIC_PMR.
	 * In addition to this, writing 255 to the GICC_PMR always sets it to the 
	 * largest supported priority field value.
	 */
	*REG_GIC_GICC_PMR = GICC_PMR_PRIO_MIN;

	/* Handle all of interrupts in a single group */
	*REG_GIC_GICC_BPR = GICC_BPR_NO_GROUP;

	/* Clear all of the active interrupts */
	for(pending_irq = ( *REG_GIC_GICC_IAR & GICC_IAR_INTR_IDMASK ); 
	    ( pending_irq != GICC_IAR_SPURIOUS_INTR );
	    pending_irq = ( *REG_GIC_GICC_IAR & GICC_IAR_INTR_IDMASK ) )
		*REG_GIC_GICC_EOIR = *REG_GIC_GICC_IAR;

	/* Enable CPU interface */
	*REG_GIC_GICC_CTLR = GICC_CTLR_ENABLE;
}

static void init_gicd(void)
{
	int	i, regs_nr;

    puts("init_gicd()\n");
	/* Diable distributor */
	*REG_GIC_GICD_CTLR = GIC_GICD_CTLR_DISABLE;

	/* Disable all IRQs */
	regs_nr = (GIC_INT_MAX + GIC_GICD_INT_PER_REG - 1) / GIC_GICD_INT_PER_REG;
	for (i = 0; regs_nr > i; ++i)
		*REG_GIC_GICD_ICENABLER(i) = ~((unsigned int)(0)); 

	/* Clear all pending IRQs */
	regs_nr = (GIC_INT_MAX + GIC_GICD_INT_PER_REG - 1) / GIC_GICD_INT_PER_REG;
	for (i = 0; regs_nr > i; ++i) 
		*REG_GIC_GICD_ICPENDR(i) = ~((unsigned int)(0));

	/* Set all of interrupt priorities as the lowest priority */
	regs_nr = ( GIC_INT_MAX + GIC_GICD_IPRIORITY_PER_REG - 1) / 
		GIC_GICD_IPRIORITY_PER_REG ;
	for (i = 0; regs_nr > i; i++)
		*REG_GIC_GICD_IPRIORITYR(i) = ~((unsigned int)(0));

	/* Set target of all of shared peripherals to processor 0 */
	for (i = GIC_INTNO_SPI0 / GIC_GICD_ITARGETSR_PER_REG;
	     ( (GIC_INT_MAX + (GIC_GICD_ITARGETSR_PER_REG - 1) ) / 
		 GIC_GICD_ITARGETSR_PER_REG ) > i; ++i) 
		*REG_GIC_GICD_ITARGETSR(i) = 
			(unsigned int)GIC_GICD_ITARGETSR_CORE0_TARGET_BMAP;

	/* Set trigger type for all peripheral interrupts level triggered */
	for (i = GIC_INTNO_PPI0 / GIC_GICD_ICFGR_PER_REG;
	     (GIC_INT_MAX + (GIC_GICD_ICFGR_PER_REG - 1)) / GIC_GICD_ICFGR_PER_REG > i; ++i)
		*REG_GIC_GICD_ICFGR(i) = GIC_GICD_ICFGR_LEVEL;

	/* Enable distributor */
	*REG_GIC_GICD_CTLR = GIC_GICD_CTLR_ENABLE;
}

/** Disable IRQ
    @param[in] irq IRQ number
 */
void gicd_disable_int(int irq) {
	*REG_GIC_GICD_ICENABLER( (irq / GIC_GICD_ICENABLER_PER_REG) ) = 
		1U << ( irq % GIC_GICD_ICENABLER_PER_REG );
}

/** Enable IRQ
    @param[in] irq IRQ number
 */
void gicd_enable_int(int irq) {

	*REG_GIC_GICD_ISENABLER( (irq / GIC_GICD_ISENABLER_PER_REG) ) =
		1U << ( irq % GIC_GICD_ISENABLER_PER_REG );

}

/** Clear a pending interrupt
    @param[in] irq IRQ number
 */
void gicd_clear_pending(int irq) {

	*REG_GIC_GICD_ICPENDR( (irq / GIC_GICD_ICPENDR_PER_REG) ) = 
		1U << ( irq % GIC_GICD_ICPENDR_PER_REG );
}


/** Probe pending interrupt
    @param[in] irq IRQ number
 */
static int gicd_probe_pending(int irq) {
	int is_pending;

	is_pending = ( *REG_GIC_GICD_ISPENDR( (irq / GIC_GICD_ISPENDR_PER_REG) ) &
	    ( 1U << ( irq % GIC_GICD_ISPENDR_PER_REG ) ) );

	return ( is_pending != 0 );
}

/** Set an interrupt target processor
    @param[in] irq IRQ number
    @param[in] p   Target processor mask
    0x1 processor 0
    0x2 processor 1
    0x4 processor 2
    0x8 processor 3
 */
static void gicd_set_target(int irq, unsigned int p){
	unsigned int  shift;
	unsigned int    reg;

	shift = (irq % GIC_GICD_ITARGETSR_PER_REG) * GIC_GICD_ITARGETSR_SIZE_PER_REG;

	reg = *REG_GIC_GICD_ITARGETSR(irq / GIC_GICD_ITARGETSR_PER_REG);
	reg &= ~( ((unsigned int)(0xff)) << shift);
	reg |= (p << shift);
	*REG_GIC_GICD_ITARGETSR(irq / GIC_GICD_ITARGETSR_PER_REG) = reg;
}

/** Set an interrupt priority
    @param[in] irq  IRQ number
    @param[in] prio Interrupt priority in Arm specific expression
 */
static void gicd_set_priority(int irq, unsigned int prio){
	unsigned int  shift;
	unsigned int    reg;

	shift = (irq % GIC_GICD_IPRIORITY_PER_REG) * GIC_GICD_IPRIORITY_SIZE_PER_REG;
	reg = *REG_GIC_GICD_IPRIORITYR(irq / GIC_GICD_IPRIORITY_PER_REG);
	reg &= ~(((unsigned int)(0xff)) << shift);
	reg |= (prio << shift);
	*REG_GIC_GICD_IPRIORITYR(irq / GIC_GICD_IPRIORITY_PER_REG) = reg;
}

/** Configure IRQ 
    @param[in] irq     IRQ number
    @param[in] config  Configuration value for GICD_ICFGR
 */
static void gicd_config(int irq, unsigned int config)
{
	unsigned int	shift; 
	unsigned int	  reg;

	shift = (irq % GIC_GICD_ICFGR_PER_REG) * GIC_GICD_ICFGR_SIZE_PER_REG; /* GICD_ICFGR has 16 fields, each field has 2bits. */

	reg = *REG_GIC_GICD_ICFGR( irq / GIC_GICD_ICFGR_PER_REG);

	reg &= ~( ( (unsigned int)(0x03) ) << shift );  /* Clear the field */
	reg |= ( ( (unsigned int)config ) << shift );  /* Set the value to the field correponding to irq */
	*REG_GIC_GICD_ICFGR( irq / GIC_GICD_ICFGR_PER_REG) = reg;
}

/** Send End of Interrupt to IRQ line for GIC
    @param[in] ctrlr   IRQ controller information
    @param[in] irq     IRQ number
 */
void gic_v3_eoi(int irq) {
	gicd_clear_pending(irq);
}

/* Initialize GIC IRQ controller */
/* RyanYao: 2018/07/20
 *    I supppose the current access is security, because GICD_CTLR.DS is 0b0 and
 *    we can access.
 */ 
void gic_v3_initialize(void)
{

    puts("gic_v3_initialize()\n");
	init_gicd();
	init_gicc();
	gicd_config(TIMER_IRQ, GIC_GICD_ICFGR_EDGE);
	gicd_set_priority(TIMER_IRQ, 0 << GIC_PRI_SHIFT );  /* Set priority */
	gicd_set_target(TIMER_IRQ, 0x1);  /* processor 0 */
	gicd_clear_pending(TIMER_IRQ);
	gicd_enable_int(TIMER_IRQ);
}


/** Find pending IRQ
    @param[in]     exc  An exception frame
    @param[in,out] irqp An IRQ number to be processed
 */
int gic_v3_find_pending_irq(struct exception_frame* exc, int *irqp) {
	int rc;
	int i;
	for( i = 0; GIC_INT_MAX > i; ++i) {
		if ( gicd_probe_pending(i) ) {
			rc = IRQ_FOUND;
			*irqp = i;
			goto found;
		}
	}

	rc = IRQ_NOT_FOUND ;
found:
	return rc;
}

