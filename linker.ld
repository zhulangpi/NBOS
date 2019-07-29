
ENTRY(_start)
SECTIONS
{
	. = 0xffff000000000000;
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
    . = ALIGN(4096);
    pg_tbl_start = .;   /* 存放内核页表 */  /* PA: 0x40086000 */

    . += 3*4096 ;
    pg_tbl_end = .;

	.bss : { *(.bss COMMON) }

    /*. = 0x40100000;*/      /* 1MB */
    . = ALIGN(4096);    /* 按页对齐，后面的全部按页管理 */
    __pages_start = .;

    /*. = 0x41000000; */    /* 16MB */
    __pages_end = .;
}

