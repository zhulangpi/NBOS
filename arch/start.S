#define _ASSEMBLY_
#include "asm/arm_asm.h"
#include "syscall.h"
#include "mmu.h"
#include "mm.h"
#include "aarch64.h"
#include "board.h"

    .globl  _start
    text_align
_start:

    mov x1, #0xff

    ldr x0, =SCTLR_VALUE_MMU_DISABLED
    msr sctlr_el1, x0

/* 
    MMU kernel map
*/
    // 在内存中创建页表， PGD一项， PUD一项，PMD 512项（每项2MB），各级各占用一页
    mov  x0, #KERNEL_VA_START   // x0  VA
    adrp x1, pg_tbl_start       // x1  PA for PGD base
    add  x2, x1, #PAGE_SIZE     // x2  PA for PUD base
    add  x3, x2, #PAGE_SIZE     // x3  PA for PMD base

    add  x4, x1, #0             // x4  PA for PGD entry
    add  x5, x2, #0             // x5  PA for PUD entry
    //populate first PGD
    orr  x6, x2, #MM_TYPE_PAGE_TABLE
    str  x6, [x4]               // populate PGD entry
    //populate first PUD
    orr  x6, x3, #MM_TYPE_PAGE_TABLE
    str  x6, [x5]               // populate PUD entry
    //populate 512 PMD
    mov  x6, #MEM_BASE          // x6  PA base
    mov  x7, #0                 // offset of VA/PA
    mov  x8, #0                 // index of PMD
1:
    lsl  x9, x8, #3             // offset of PMD
    add  x10, x6, x7            // PA
    mov  x11, #MMU_FLAGS
    orr  x10, x10, x11          // PME
    add  x12, x3, x9
    str  x10, [x12]

    add  x7, x7, #SECTION_SIZE  // offset of PA
    add  x8, x8, #1
    cmp  x8, #509               // 510>509则跳转，511、512table 给 中断控制器和串口控制器
    b.ls    1b                  // <=

//UART REG，511号entry，对应VA = VA_START + (511-1) * 2 * 1024 * 1024
    lsl  x9, x8, #3
    mov  x10, #UART_PA_BASE
    mov  x11, #MMU_DEVICE_FLAGS
    orr  x10, x10, x11
    add  x12, x3,  x9
    str  x10, [x12]

//GIC REG, 512号entry，对应VA = VA_START + (512-1)*2*1024*1024
    add  x8, x8, #1
    lsl  x9, x8, #3
    mov  x10, #QEMU_VIRT_GIC_PA_BASE
    mov  x11, #MMU_DEVICE_FLAGS
    orr  x10, x10, x11 
    add  x12, x3,  x9  
    str  x10, [x12]



/*
    MMU DRAM map
*/
    mov  x0, #MEM_BASE
    adrp x1, ram_tbl_start      // x1  PA for PGD base
    add  x2, x1, #PAGE_SIZE     // x2  PA for PUD base
    add  x3, x2, #PAGE_SIZE     // x3  PA for PMD base

    add  x4, x1, #0             // x4  PA for PGD entry
    add  x5, x2, #8             // x5  PA for PUD entry, second entry
    //populate second PGD, 1G-2G
    orr  x6, x2, #MM_TYPE_PAGE_TABLE
    str  x6, [x4]               // populate PGD entry
    //populate first PUD
    orr  x6, x3, #MM_TYPE_PAGE_TABLE
    str  x6, [x5]               // populate PUD entry
    //populate 1 PMD, map 0x4000 0000 - 0x4020 0000(DRAM) to 0x4000 0000 - 0x4020 0000(VA)
    mov  x6, #MEM_BASE          // x6  PA base
    mov  x7, #0                 // offset of VA/PA
    mov  x8, #0                 // index of PMD
    lsl  x9, x8, #3             // offset of PMD
    add  x10, x6, x7            // PA
    mov  x11, #MMU_FLAGS
    orr  x10, x10, x11          // PME
    add  x12, x3, x9
    str  x10, [x12]

    add  x7, x7, #SECTION_SIZE  // offset of PA
    add  x8, x8, #1


/*
    config MMU
*/
    adrp    x0,  pg_tbl_start    //pc 相对寻址，获得物理地址，此时MMU还没开
    msr ttbr1_el1, x0
    adrp    x0,  ram_tbl_start    //pc 相对寻址，获得物理地址，此时MMU还没开
    msr ttbr0_el1, x0

    ldr x0, =(TCR_VALUE)    
    msr tcr_el1, x0

    ldr x0, =(MAIR_VALUE)
    msr mair_el1, x0

    //Initialize VBAR_EL1
    ldr x0, =vector_table_el1   //x2 获得链接地址
    msr vbar_el1,   x0

    /* configure init kernel task stack */
    ldr x0, =__init_stack_top
    mov sp, x0                  //sp_el1

    ldr x2, =init_main          //x2 获得链接地址

    mrs x0, s3_1_c15_c2_1
    orr x0, x0, #(0x1<<6)       //cpuectlr.smpen = 1
    msr s3_1_c15_c2_1, x0
    mrs x0, sctlr_el1
//    orr x0, x0, #(0x1 << 2)       // D bit, dcache
//    orr x0, x0, #(0x1 << 12)      // I bit, icache
    orr x0, x0, #1              // M bit, mmu
    msr sctlr_el1, x0
//    dsb sy
//    isb

    br x2
