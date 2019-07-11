#ifndef __REG_H__
#define __REG_H__

#define UART_BASE   (0X9000000)   
#define UART_DATA	(*(volatile unsigned long*)(UART_BASE))

#endif
