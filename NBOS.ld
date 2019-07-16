
ENTRY(_start)
SECTIONS
{
    __ram_addr_min = .;
	. = 0x40000000;
/*    .start : { start.o(.text)  }  */
	.text : { 
        arch/start.o  (CODE_ENTRY)
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

    .stacks : { *(.stacks) }    /* 内核栈 */

    . = 0x40100000;      /* 1MB */
    . = ALIGN(4096);    /* 按页对齐，后面的全部作为堆管理 */
    __pages_start = .;

    . = 0x41000000;     /* 16MB */
    __pages_end = .;
    __ram_addr_end = .;
}

