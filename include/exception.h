#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_


/*
 * IRQ
 */
#define IRQ_FOUND          (1)
#define IRQ_NOT_FOUND      (0)


#define ESR_EC_MASK     (0x3f<<26)
#define ESR_EC_SVC      (0x15<<26)
#define ESR_ISS_MASK    (0x1ffffff)
#define ESR_SVC_IMM16_MASK  (0xffff)    //系统调用号

extern void put_exception(unsigned long no);
extern void exception_exception(unsigned int esr_el1);
extern void irq_dispatch(void);

#endif /* _EXCEPTION_H_ */
