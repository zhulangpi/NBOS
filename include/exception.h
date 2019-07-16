/* -*- mode: asm; coding:utf-8 -*- */
/************************************************************************/
/*  OS kernel sample                                                    */
/*  Copyright 2014 Takeharu KATO                                        */
/*                                                                      */
/************************************************************************/

#if !defined(_EXCEPTION_H)
#define _EXCEPTION_H 

/* Vector Table
 * see 5.1.1 Setting up a vector table in
 * Application Note Bare-metal Boot Code for ARMv8-A Processors Version 1.0
 */

/*
 * AArch64 exception types
 */
/* Current EL with SP0 */
#define AARCH64_EXC_SYNC_SP0      (0x1)   /* Synchronous */
#define AARCH64_EXC_IRQ_SP0       (0x2)   /* IRQ/vIRQ */
#define AARCH64_EXC_FIQ_SP0       (0x3)   /* FIQ/vFIQ */
#define AARCH64_EXC_SERR_SP0      (0x4)   /* SError/vSError */
/* Current EL with SPx */
#define AARCH64_EXC_SYNC_SPX      (0x11)
#define AARCH64_EXC_IRQ_SPX       (0x12)
#define AARCH64_EXC_FIQ_SPX       (0x13)
#define AARCH64_EXC_SERR_SPX      (0x14)
/* Lower EL using AArch64 */
#define AARCH64_EXC_SYNC_AARCH64  (0x21)
#define AARCH64_EXC_IRQ_AARCH64   (0x22)
#define AARCH64_EXC_FIQ_AARCH64   (0x23)
#define AARCH64_EXC_SERR_AARCH64  (0x24)
/* Lower EL using AArch32 */
#define AARCH64_EXC_SYNC_AARCH32  (0x31)
#define AARCH64_EXC_IRQ_AARCH32   (0x32)
#define AARCH64_EXC_FIQ_AARCH32   (0x33)
#define AARCH64_EXC_SERR_AARCH32  (0x34)


/*
 * exception_frame offset definitions
 */
#define EXC_FRAME_SIZE (288)	/* sizeof(struct _exception_frame) */
#define EXC_EXC_TYPE_OFFSET (0)	/* __asm_offsetof(struct _exception_frame, exc_type) */
#define EXC_EXC_ESR_OFFSET (8)	/* __asm_offsetof(struct _exception_frame, exc_esr) */
#define EXC_EXC_SP_OFFSET (16)	/* __asm_offsetof(struct _exception_frame, exc_sp) */
#define EXC_EXC_ELR_OFFSET (24)	/* __asm_offsetof(struct _exception_frame, exc_elr) */
#define EXC_EXC_SPSR_OFFSET (32)/* __asm_offsetof(struct _exception_frame, exc_spsr) */

/*
 * IRQ
 */
#define IRQ_FOUND          (1)
#define IRQ_NOT_FOUND      (0)


#define ESR_EC_MASK     (0x3f<<26)
#define ESR_ISS_MASK    (0x1ffffff)

#define ESR_EC_SVC      (0x15<<26)
#define ESR_SVC_IMM16_MASK  (0xffff)


struct exception_frame{
	unsigned long exc_type;
	unsigned long exc_esr;
	unsigned long exc_sp;
	unsigned long exc_elr;
	unsigned long exc_spsr;
	unsigned long x0;
	unsigned long x1;
	unsigned long x2;
	unsigned long x3;
	unsigned long x4;
	unsigned long x5;
	unsigned long x6;
	unsigned long x7;
	unsigned long x8;
	unsigned long x9;
	unsigned long x10;
	unsigned long x11;
	unsigned long x12;
	unsigned long x13;
	unsigned long x14;
	unsigned long x15;
	unsigned long x16;
	unsigned long x17;
	unsigned long x18;
	unsigned long x19;
	unsigned long x20;
	unsigned long x21;
	unsigned long x22;
	unsigned long x23;
	unsigned long x24;
	unsigned long x25;
	unsigned long x26;
	unsigned long x27;
	unsigned long x28;
	unsigned long x29;
	unsigned long x30;
};


extern void common_trap_handler(struct exception_frame *_exc);
#endif /* _EXCEPTION_H */
