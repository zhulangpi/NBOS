#ifndef _ARM_ASM_H_
#define _ARM_ASM_H_

#define vector_table_align .align 11    /* Vector tables must be placed at a 2KB-aligned address */
#define vector_entry_align .align 7     /* Each entry is 128B in size*/
#define text_align .align  2            /* Text alignment */


#endif
