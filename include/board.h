/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Board specific definitions                                        */
/*                                                                    */
/**********************************************************************/
#if !defined(_BOARD_H)
#define  _BOARD_H 

/*
 * GIC on QEMU Virt 
 */
#define QEMU_VIRT_GIC_PA_BASE	    (0x08000000)
#define QEMU_VIRT_GIC_BASE			(0xffff00003fe00000)
#define QEMU_VIRT_GIC_INT_MAX		(64)
#define QEMU_VIRT_GIC_PRIO_MAX		(16)
/* SGI: Interrupt IDs 0-15 */
/* PPI: Interrupt IDs 16-31 */
/* SPI: Interrupt IDs 32-63 */
#define QEMU_VIRT_GIC_INTNO_SGIO	(0)
#define QEMU_VIRT_GIC_INTNO_PPIO	(16)
#define QEMU_VIRT_GIC_INTNO_SPIO	(32)

#define GIC_BASE					(QEMU_VIRT_GIC_BASE)
#define GIC_INT_MAX					(QEMU_VIRT_GIC_INT_MAX)
#define GIC_PRIO_MAX				(QEMU_VIRT_GIC_PRIO_MAX)
#define GIC_INTNO_SGI0				(QEMU_VIRT_GIC_INTNO_SGIO)
#define GIC_INTNO_PPI0				(QEMU_VIRT_GIC_INTNO_PPIO)
#define GIC_INTNO_SPI0				(QEMU_VIRT_GIC_INTNO_SPIO)

#define GIC_PRI_SHIFT				(4)
#define GIC_PRI_MASK				(0x0f)

#define TIMER_IRQ					(27)  /** Timer IRQ  */


// UART
#define UART_PA_BASE                (0x09000000)
#define UART_BASE                   (0xffff00003fc00000)   
#define UART_DATA                   (*(volatile unsigned long*)(UART_BASE))


// PFLASH
#define PFLASH1_PA_BASE				(0x04000000)
#define PFLASH1_BASE				(0xffff000400000000)
#define PFLASH1_SIZE				(64*1024*1024)

#endif  /* _BOARD_H */
