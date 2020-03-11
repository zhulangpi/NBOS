#include "fs.h"
#include "minix_fs.h"
#include "lib.h"

struct block_device *root_bdev = NULL;

static int pflash_direct_read(sector_t sec, unsigned long *addr);
static int pflash_direct_write(sector_t sec, unsigned long *addr);


struct blkdev_operations pflash_ops = {
    .direct_read = pflash_direct_read,
    .direct_write = pflash_direct_write,
};

struct gendisk pflash = {
    .blkdev_op = &pflash_ops,
//    .diskname = "pflash1",
};


void init_fs(void)
{

    root_bdev = (struct block_device*)kmalloc(sizeof(struct block_device));

    root_bdev->genhd = &pflash;
    root_bdev->sb = (struct super_block*)kmalloc(sizeof(struct super_block));
    root_bdev->sb->s_private = alloc_minix_sb(root_bdev);


}


static int pflash_direct_read(sector_t sec, unsigned long *addr)
{
    return 0;
}


static int pflash_direct_write(sector_t sec, unsigned long *addr)
{
    return 0;
}


