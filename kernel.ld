/* With QEMU this is not needed */
/* ENTRY(_Reset) */
SECTIONS
{
/*
* QEMU bootloader will put it on 0x40080000 anyway, this is needed for
* sections laying down, not loading
*/

	. = 0x40000000; 
/*    .start : { start.o(.text)  }  */
	.text : { 
        start.o  (INTERRUPT_VECTOR)
        *(.text) 
    }
    
    .data : {
        *(.data)
        *(.rodata)
    }

	.bss : { *(.bss COMMON) }

    . = ALIGN(4096); /* 4KB align */

    .stacks : { *(.stacks) }

    . = ALIGN(8);

    . = . + 0x1000; /* 4kB of stack memory */
    stack_top = .; /* stack grows backward towards .bss */
}

