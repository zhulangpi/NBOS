#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_


/*
 * IRQ
 */
#define IRQ_FOUND          (1)
#define IRQ_NOT_FOUND      (0)


#define FRAME_SZ        (17*16)         //在kernel_entry中保存了一共34(17*2)个寄存器的值
#define X0_OFFSET       (FRAME_SZ)      //x0保存在异常栈帧的首地址


#define ESR_EC_SHIFT    (26)
#define ESR_EC_MASK     (0x3f<<ESR_EC_SHIFT)
#define ESR_EC_SVC      (0x15<<ESR_EC_SHIFT)
#define ESR_ISS_MASK    ((1<<24)-1)
#define ESR_SVC_IMM16_MASK  (0xffff)    //系统调用号

#ifndef _ASSEMBLY_

extern void put_exception(unsigned long no, unsigned long lr, unsigned int esr);
extern void exception_exception(unsigned int esr_el1);
extern void irq_dispatch(void);

#endif

#endif /* _EXCEPTION_H_ */
