/* With QEMU this is not needed */
/* ENTRY(_Reset) */
SECTIONS
{
/*
* QEMU bootloader will put it on 0x40080000 anyway, this is needed for
* sections laying down, not loading
*/
    __ram_addr_min = .;
	. = 0x40000000;
/*    .start : { start.o(.text)  }  */
	.text : { 
        init/start.o  (INTERRUPT_VECTOR)
        *(.text) 
    }
    
    .data : {
/*        *(.data) */

        . = ALIGN(4096);
        /*  
         * first, the init task union, aligned
         * to an 4096 byte boundary.
         */  
        *(.data.init_stack)
        __init_stack_top = .;

        *(.rodata)
    }

	.bss : { *(.bss COMMON) }

    . = ALIGN(4096); /* 4KB align */

    .stacks : { *(.stacks) }

    . = ALIGN(4096);    /* 按页对齐，后面的全部作为堆管理 */
    __heap_start = .;



    . = 0x80000000;
    __heap_end = .;
    __ram_addr_end = .;
}

