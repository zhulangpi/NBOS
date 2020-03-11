#include "minix_fs.h"
#include "fs.h"
#include "lib.h"


//读取外存上的minix系统的超级块
struct minix_super_block* alloc_minix_sb(struct block_device *bd)
{
    struct minix_super_block *sb = NULL;
    void* block = kmalloc(BLOCK_SZ);

    sb = (struct minix_super_block*)kmalloc(sizeof(struct minix_super_block));

    bd->genhd->blkdev_op->direct_read(SB_OFFSET, block);

    memcpy(sb, block, sizeof(struct minix_super_block));


    return sb;
}



