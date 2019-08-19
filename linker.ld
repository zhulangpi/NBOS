
ENTRY(_start)
SECTIONS
{
	. = 0xffff000000080000;
	.text.boot : { 
        arch/start.o
    }
    . = ALIGN(4096);
    .text : { 
        *(.text)
    }
    .data : {
        . = ALIGN(4096);
        *(.data.init_stack)
        __init_stack_top = .;
        *(.data)
    }
    .rodata : { *(.rodata) }
	.bss : { *(.bss*) }

    . = ALIGN(4096);
    pg_tbl_start = .;   /* 存放内核页表 */  /* PA: 0x4008 6000 */
    . += 3*4096 ;
    pg_tbl_end = .;

    ram_tbl_start = .;  /* 建立DRAM的1:1映射 */  /* PA: 0x4008 9000 */
    . += 3*4096;
    ram_tbl_end = .;

	__img_end = .;
}

