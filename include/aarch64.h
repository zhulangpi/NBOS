/* -*- mode: C; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  AArch64 definitions                                               */
/*                                                                    */
/**********************************************************************/
#if !defined(_AARCH64_H)
#define  _AARCH64_H 

/* CurrentEL, Current Exception Level */
#define CURRENT_EL_MASK		0x3
#define CURRENT_EL_SHIFT	2

/* DAIF, Interrupt Mask Bits */
#define DAIF_DBG_BIT		(1<<3)	/* Debug mask bit */
#define DAIF_ABT_BIT		(1<<2)	/* Asynchronous abort mask bit */
#define DAIF_IRQ_BIT		(1<<1)	/* IRQ mask bit */
#define DAIF_FIQ_BIT		(1<<0)	/* FIQ mask bit */

/*
 * Interrupt flags
 */
#define AARCH64_DAIF_FIQ	(1)		/* FIQ */
#define AARCH64_DAIF_IRQ	(2)		/* IRQ */

/* Timer */  
#define CNTV_CTL_ENABLE		(1 << 0)	/* Enables the timer */	
#define CNTV_CTL_IMASK		(1 << 1)	/* Timer interrupt mask bit */
#define CNTV_CTL_ISTATUS	(1 << 2)	/* The status of the timer interrupt. This bit is read-only */

/* Wait For Interrupt */
#define wfi()		asm volatile("wfi" : : : "memory")

/* PSTATE and special purpose register access functions */
unsigned int raw_read_current_el(void);
unsigned int get_current_el(void);
unsigned int raw_read_daif(void);
void raw_write_daif(unsigned int daif);
void enable_debug_exceptions(void);
void enable_serror_exceptions(void);
void enable_irq(void);
void enable_fiq(void);
void disable_debug_exceptions(void);
void disable_serror_exceptions(void);
void disable_irq(void);
void disable_fiq(void);
/* SPSR_EL1, Saved Program Status Register (EL1) */
unsigned int raw_read_spsr_el1(void);
void raw_write_spsr_el1(unsigned int spsr_el1);
/* ISR_EL1, Interrupt Status Register */
unsigned int raw_read_isr_el1(void);
unsigned long raw_read_rvbar_el1(void);
void raw_write_rvbar_el1(unsigned long rvbar_el1);
unsigned long raw_read_vbar_el1(void);
void raw_write_vbar_el1(unsigned long vbar_el1);

/* CNTV_CTL_EL0, Counter-timer Virtual Timer Control register */
unsigned int raw_read_cntv_ctl(void);
void disable_cntv(void);
void enable_cntv(void);
/* CNTFRQ_EL0, Counter-timer Frequency register */
unsigned int raw_read_cntfrq_el0(void);
void raw_write_cntfrq_el0(unsigned int cntfrq_el0);
/* CNTVCT_EL0, Counter-timer Virtual Count register */
unsigned long raw_read_cntvct_el0(void);
/* CNTV_CVAL_EL0, Counter-timer Virtual Timer CompareValue register */
unsigned long raw_read_cntv_cval_el0(void);
void raw_write_cntv_cval_el0(unsigned long cntv_cval_el0);

#endif  /*  _AARCH64_H   */
