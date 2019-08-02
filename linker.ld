
ENTRY(_start)
SECTIONS
{
	. = 0xffff000000080000;
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
    pg_tbl_start = .;   /* 存放内核页表 */  /* PA: 0x4008 6000 */

    . += 3*4096 ;
    pg_tbl_end = .;


    ram_tbl_start = .;  /* 建立DRAM的1:1映射 */  /* PA: 0x4008 9000 */

    . += 3*4096;
    ram_tbl_end = .;

	.bss : { *(.bss COMMON) }
    __img_end = .;
}

