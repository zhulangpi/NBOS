/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  PrimeCell Generic Interrupt Controller (PL390)                    */
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
#if !defined(_GIC_V3_H)
#define _GIC_V3_H 



#define GIC_GICD_BASE		(GIC_BASE)  /* GICD MMIO base address */
#define GIC_GICC_BASE		(GIC_BASE + 0x10000) /* GICC MMIO base address */

#define GIC_GICD_INT_PER_REG			(32)	/* 32 interrupts per reg */
#define GIC_GICD_IPRIORITY_PER_REG		(4)		/* 4 priority per reg */
#define GIC_GICD_IPRIORITY_SIZE_PER_REG	(8) 	/* priority element size */
#define GIC_GICD_ITARGETSR_CORE0_TARGET_BMAP (0x01010101) /* CPU interface 0 */
#define GIC_GICD_ITARGETSR_PER_REG		(4) 
#define GIC_GICD_ITARGETSR_SIZE_PER_REG	(8) 
#define GIC_GICD_ICFGR_PER_REG			(16) 
#define GIC_GICD_ICFGR_SIZE_PER_REG		(2) 
#define GIC_GICD_ICENABLER_PER_REG		(32)
#define GIC_GICD_ISENABLER_PER_REG		(32)
#define GIC_GICD_ICPENDR_PER_REG		(32)
#define GIC_GICD_ISPENDR_PER_REG		(32)

/* 8.12 The GIC CPU interface register map */
#define GIC_GICC_CTLR     (GIC_GICC_BASE + 0x000) /* CPU Interface Control Register */
#define GIC_GICC_PMR      (GIC_GICC_BASE + 0x004) /* Interrupt Priority Mask Register */
#define GIC_GICC_BPR      (GIC_GICC_BASE + 0x008) /* Binary Point Register */
#define GIC_GICC_IAR      (GIC_GICC_BASE + 0x00C) /* Interrupt Acknowledge Register */
#define GIC_GICC_EOIR     (GIC_GICC_BASE + 0x010) /* End of Interrupt Register */
#define GIC_GICC_RPR      (GIC_GICC_BASE + 0x014) /* Running Priority Register */
#define GIC_GICC_HPIR     (GIC_GICC_BASE + 0x018) /* Highest Pending Interrupt Register */
#define GIC_GICC_ABPR     (GIC_GICC_BASE + 0x01C) /* Aliased Binary Point Register */
#define GIC_GICC_IIDR     (GIC_GICC_BASE + 0x0FC) /* CPU Interface Identification Register */

/* 8.13.7 GICC_CTLR, CPU Interface Control Register */
#define GICC_CTLR_ENABLE			(0x1)	/* Enable GICC */
#define GICC_CTLR_DISABLE			(0x0)	/* Disable GICC */

/* 8.13.14 GICC_PMR, CPU Interface Priority Mask Register */
#define GICC_PMR_PRIO_MIN			(0xff)	/* The lowest level mask */
#define GICC_PMR_PRIO_HIGH			(0x0)	/* The highest level mask */

/* 8.13.6 GICC_BPR, CPU Interface Binary Point Register */
/* In systems that support only one Security state, when GICC_CTLR.CBPR == 0, 
this register determines only Group 0 interrupt preemption. */
#define GICC_BPR_NO_GROUP			(0x0)	/* handle all interrupts */

/* 8.13.11 GICC_IAR, CPU Interface Interrupt Acknowledge Register */
#define GICC_IAR_INTR_IDMASK		(0x3ff)	/* 0-9 bits means Interrupt ID */
#define GICC_IAR_SPURIOUS_INTR		(0x3ff)	/* 1023 means spurious interrupt */

/* 8.8 The GIC Distributor register map */
#define GIC_GICD_CTLR			(GIC_GICD_BASE + 0x000)					/* Distributor Control Register */
#define GIC_GICD_TYPER			(GIC_GICD_BASE + 0x004)					/* Interrupt Controller Type Register */
#define GIC_GICD_IIDR			(GIC_GICD_BASE + 0x008)					/* Distributor Implementer Identification Register */
#define GIC_GICD_IGROUPR(n)		(GIC_GICD_BASE + 0x080 + ( (n) * 4 ) )	/* Interrupt Group Registers */
#define GIC_GICD_ISENABLER(n)	(GIC_GICD_BASE + 0x100 + ( (n) * 4 ) )	/* Interrupt Set-Enable Registers */
#define GIC_GICD_ICENABLER(n)	(GIC_GICD_BASE + 0x180 + ( (n) * 4 ) )	/* Interrupt Clear-Enable Registers */
#define GIC_GICD_ISPENDR(n)		(GIC_GICD_BASE + 0x200 + ( (n) * 4 ) )	/* Interrupt Set-Pending Registers */
#define GIC_GICD_ICPENDR(n)		(GIC_GICD_BASE + 0x280 + ( (n) * 4 ) )	/* Interrupt Clear-Pending Registers */
#define GIC_GICD_ISACTIVER(n)	(GIC_GICD_BASE + 0x300 + ( (n) * 4 ) )	/* Interrupt Set-Active Registers */
#define GIC_GICD_ICACTIVER(n)	(GIC_GICD_BASE + 0x380 + ( (n) * 4 ) )	/* Interrupt Clear-Active Registers */
#define GIC_GICD_IPRIORITYR(n)	(GIC_GICD_BASE + 0x400 + ( (n) * 4 ) )	/* Interrupt Priority Registers */
#define GIC_GICD_ITARGETSR(n)	(GIC_GICD_BASE + 0x800 + ( (n) * 4 ) )	/* Interrupt Processor Targets Registers */
#define GIC_GICD_ICFGR(n)		(GIC_GICD_BASE + 0xc00 + ( (n) * 4 ) )	/* Interrupt Configuration Registers */
#define GIC_GICD_NSCAR(n)		(GIC_GICD_BASE + 0xe00 + ( (n) * 4 ) )	/* Non-secure Access Control Registers */
#define GIC_GICD_SGIR			(GIC_GICD_BASE + 0xf00 )				/* Software Generated Interrupt Register */
#define GIC_GICD_CPENDSGIR(n)	(GIC_GICD_BASE + 0xf10 + ( (n) * 4 ) )	/* SGI Clear-Pending Registers */
#define GIC_GICD_SPENDSGIR(n)	(GIC_GICD_BASE + 0xf20 + ( (n) * 4 ) )	/* SGI Set-Pending Registers */

/* 8.9.4 GICD_CTLR, Distributor Control Register */
#define GIC_GICD_CTLR_ENABLE	(0x1)	/* Enable GICD */
#define GIC_GICD_CTLR_DISABLE	(0x0)	/* Disable GICD */

/* 8.9.7 GICD_ICFGR<n>, Interrupt Configuration Registers */
#define GIC_GICD_ICFGR_LEVEL	(0x0)	/* level-sensitive */
#define GIC_GICD_ICFGR_EDGE		(0x2)	/* edge-triggered */

/* Register access macros for GICC */
#define REG_GIC_GICC_CTLR          ((volatile unsigned int *)(unsigned long)GIC_GICC_CTLR)
#define REG_GIC_GICC_PMR           ((volatile unsigned int *)(unsigned long)GIC_GICC_PMR)
#define REG_GIC_GICC_BPR           ((volatile unsigned int *)(unsigned long)GIC_GICC_BPR)
#define REG_GIC_GICC_IAR           ((volatile unsigned int *)(unsigned long)GIC_GICC_IAR)
#define REG_GIC_GICC_EOIR          ((volatile unsigned int *)(unsigned long)GIC_GICC_EOIR)
#define REG_GIC_GICC_RPR           ((volatile unsigned int *)(unsigned long)GIC_GICC_RPR)
#define REG_GIC_GICC_HPIR          ((volatile unsigned int *)(unsigned long)GIC_GICC_HPIR)
#define REG_GIC_GICC_ABPR          ((volatile unsigned int *)(unsigned long)GIC_GICC_ABPR)
#define REG_GIC_GICC_IIDR          ((volatile unsigned int *)(unsigned long)GIC_GICC_IIDR)

/* Register access macros for GICD */
#define REG_GIC_GICD_CTLR             ((volatile unsigned int *)(unsigned long)GIC_GICD_CTLR)
#define REG_GIC_GICD_TYPE             ((volatile unsigned int *)(unsigned long)GIC_GICD_TYPE)
#define REG_GIC_GICD_IIDR             ((volatile unsigned int *)(unsigned long)GIC_GICD_IIDR)
#define REG_GIC_GICD_IGROUPR(n)       ((volatile unsigned int *)(unsigned long)GIC_GICD_IGROUPR(n))
#define REG_GIC_GICD_ISENABLER(n)     ((volatile unsigned int *)(unsigned long)GIC_GICD_ISENABLER(n))
#define REG_GIC_GICD_ICENABLER(n)     ((volatile unsigned int *)(unsigned long)GIC_GICD_ICENABLER(n))
#define REG_GIC_GICD_ISPENDR(n)       ((volatile unsigned int *)(unsigned long)GIC_GICD_ISPENDR(n))
#define REG_GIC_GICD_ICPENDR(n)       ((volatile unsigned int *)(unsigned long)GIC_GICD_ICPENDR(n))
#define REG_GIC_GICD_ISACTIVER(n)     ((volatile unsigned int *)(unsigned long)GIC_GICD_ISACTIVER(n))
#define REG_GIC_GICD_ICACTIVER(n)     ((volatile unsigned int *)(unsigned long)GIC_GICD_ICACTIVER(n))
#define REG_GIC_GICD_IPRIORITYR(n)    ((volatile unsigned int *)(unsigned long)GIC_GICD_IPRIORITYR(n))
#define REG_GIC_GICD_ITARGETSR(n)     ((volatile unsigned int *)(unsigned long)GIC_GICD_ITARGETSR(n))
#define REG_GIC_GICD_ICFGR(n)         ((volatile unsigned int *)(unsigned long)GIC_GICD_ICFGR(n))
#define REG_GIC_GICD_NSCAR(n)         ((volatile unsigned int *)(unsigned long)GIC_GICD_NSCAR(n))
#define REG_GIC_GICD_SGIR             ((volatile unsigned int *)(unsigned long)GIC_GICD_SGIR)
#define REG_GIC_GICD_CPENDSGIR(n)     ((volatile unsigned int *)(unsigned long)GIC_GICD_CPENDSGIR(n))
#define REG_GIC_GICD_SPENDSGIR(n)     ((volatile unsigned int *)(unsigned long)GIC_GICD_SPENDSGIR(n))

void gic_v3_initialize(void);
void gic_v3_eoi(int irq);
int gic_v3_find_pending_irq(int *irqp);
void gicd_disable_int(int irq);
void gicd_enable_int(int irq);
void gicd_clear_pending(int irq);
#endif  /* _GIC_V3_H */
