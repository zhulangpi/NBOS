#include "fs.h"
#include "minix_fs.h"
#include "lib.h"
#include "printf.h"
#include "board.h"
#include "list.h"

struct block_device *root_bdev = NULL;

static int pflash_direct_read(sector_t sect, unsigned long *addr);
static int pflash_direct_write(sector_t sect, unsigned long *addr);
static int pflash_read(sector_t, struct buffer_head *bh);
static int pflash_write(sector_t, struct buffer_head *bh);


struct blkdev_operations pflash_ops = {
    .read = pflash_read,
    .write = pflash_write,
    .direct_read = pflash_direct_read,
    .direct_write = pflash_direct_write,
};


struct gendisk pflash = {
    .sector_sz =  BLOCK_SZ,
    .nr_sector = PFLASH1_SIZE / BLOCK_SZ,
    .blkdev_op = &pflash_ops,
    .diskname = "pflash1",
};


void init_fs(void)
{
    root_bdev = (struct block_device*)kmalloc(sizeof(struct block_device));

    root_bdev->genhd = &pflash;
    root_bdev->sb = (struct super_block*)kmalloc(sizeof(struct super_block));
    root_bdev->sb->s_private = alloc_minix_sb(root_bdev);

}

static int pflash_direct_read(sector_t sect, unsigned long *addr)
{
    memcpy( (void*)addr, (void*)(PFLASH1_BASE + sect * BLOCK_SZ), BLOCK_SZ);

    return 0;
}


static int pflash_direct_write(sector_t sect, unsigned long *addr)
{
    memcpy( (void*)(PFLASH1_BASE + sect * BLOCK_SZ), (void*)addr, BLOCK_SZ);
    return 0;
}


static int pflash_read(sector_t sect, struct buffer_head *bh)
{
    return pflash_direct_read( sect, bh->data);
}


static int pflash_write(sector_t sect, struct buffer_head *bh)
{
    return pflash_direct_write( sect, bh->data);
}


void print_root_bdev(void)
{
    print_minix(root_bdev);
}


//定义所有buffer_head的LRU链表的头节点
LIST_HEAD(bh_head);


//为指定的块分配一个buffer_head和block
//现在的block从kmallc中分配，如果实现page cache，
//可以两者结合，从page cache层来管理
//利用page cache，读写一个文件应以页为单位，再利用buffer head来处理对设备的写入

//分配时还应该传入设备信息，以具体其读写信息，目前固定为root_bdev
struct buffer_head* alloc_buffer_head(unsigned long blk_no)
{
    struct buffer_head *bh;

    bh = (struct buffer_head*)kmalloc(sizeof(struct buffer_head));
    bh->data = kmalloc(BLOCK_SZ);

    bh->bd = root_bdev;
    bh->bd->genhd->blkdev_op->read( blk_no, bh );
    bh->b_state = BH_Uptodate;
    bh->blk_no = blk_no;

    list_add( &bh->lru, &bh_head);     //LRU链表，新节点加到最前面

    return bh;
}


// 为了方便的查找到指定块号对应的buffer_head，可以用hash链表来加速查找(暂不实现)
struct buffer_head* get_blk(unsigned long blk_no)
{
    struct list_head *pos;
    struct buffer_head *bh;

    list_for_each(pos, &bh_head){
        bh = list_entry(pos, struct buffer_head, lru);
        if(bh->blk_no == blk_no)
            return bh;
    }
    //目前不存在，新建
    bh = alloc_buffer_head(blk_no);

    return bh;
}


//先同步磁盘，再分配释放block和buffer head
int delete_buffer_head(struct buffer_head * bh)
{
    if(bh->b_state == BH_Dirty)
        bh->bd->genhd->blkdev_op->write( bh->blk_no, bh ); 

    kfree(bh->data);
    list_del(&bh->lru);
    kfree(bh);

    return 0;
}


