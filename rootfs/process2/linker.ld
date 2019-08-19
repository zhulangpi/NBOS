ENTRY(main)
SECTIONS
{
    . = 0x0;
	.main : {
		main.o
	}
    .text : { 
        *(.text)
    }   
    .data : { 
        *(.data)
    }   
    .rodata : { *(.rodata) }
    .bss : { *(.bss*) }

}

