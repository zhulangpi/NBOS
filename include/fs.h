#ifndef _FS_H
#define _FS_H

#include "type.h"
#include "list.h"
#include "mm.h"

/* 暂时不实现
struct file_operations{
}

struct file{
    struct file_operations* f_op;
};
*/

// file->inode->block_device->super_block
// 对任意inode的字节的读写都会转换为对内存中一个block的读写，
// block再以扇区为单位读写磁盘
// 两次转换：
// 1. inode offset -> block && offset
// 2. block -> 磁盘sector

// 每个inode存放了其所有的block号，只要通过block号得到buffer_head就能得到block
// 解决inode offset -> block offset的转换

// buffer_head记录磁盘块号
// 解决block -> sector 的转换


struct block_device;
struct buffer_head;


struct inode;

struct inode_operations {
    struct inode* (*lookup)(struct inode *dir, char *name);  //根据当前目录inode和路径文件名搜索inode

};

struct inode{
    struct list_head list;       //用于链接同一block_device的所有inode
    struct inode_operations *i_op;
    struct super_block *sb;
//    void *i_private;            //fs or device private pointer ,指向具体文件系统的inode
};


struct super_operations{
    struct inode *(*alloc_inode)(struct super_block *sb);
//    void (*destroy_inode)(struct inode *);
//    int (*write_inode) (struct inode *, struct writeback_control *wbc);
//    void (*put_super) (struct super_block *);
};


struct super_block{
    struct super_operations *s_op;
    struct block_device *bd;
    void  *s_fs_info;           /* Filesystem private info */
    struct list_head s_inodes;    //所有inode的链表
};


//block必须>=sector
struct blkdev_operations{
    int (*read) (sector_t sect, struct buffer_head *bh);    //读一个完整block
    int (*write) (sector_t sect, struct buffer_head *bh);   //写一个完整block
};


//管理一个物理块设备，实现对其读写，只支持一个分区
struct gendisk{
    unsigned long sector_sz;
    sector_t nr_sector;
    struct blkdev_operations *blkdev_op;    //块设备的读写操作集
//    struct request_queue *queue;
    char diskname[30];
    
};


//管理一个内核块设备，一般是一个分区
struct block_device{
    unsigned long nr_blk;   //这个块设备一共多少个block，内核block大小固定为1024B
    struct super_block *sb; //分区超级块
    struct gendisk *genhd;  //物理块设备
};


enum bh_state_bits {
    BH_Uptodate,    /* Contains valid data */
    BH_Dirty,   /* Is dirty */
    BH_Lock,    /* Is locked */
    BH_Req,     /* Has been submitted for I/O */
    BH_Uptodate_Lock,/* Used by the first bh in a page, to serialise
              * IO completion of other buffers in the page
              */

    BH_Mapped,  /* Has a disk mapping */
    BH_New,     /* Disk mapping was newly created by get_block */
    BH_Async_Read,  /* Is under end_buffer_async_read I/O */
    BH_Async_Write, /* Is under end_buffer_async_write I/O */
    BH_Delay,   /* Buffer is not yet allocated on disk */
    BH_Boundary,    /* Block is followed by a discontiguity */
    BH_Write_EIO,   /* I/O error on write */
    BH_Ordered, /* ordered write */
    BH_Eopnotsupp,  /* operation not supported (barrier) */
    BH_Unwritten,   /* Buffer is allocated on disk but not written */

    BH_PrivateStart,/* not a state bit, but the first bit available
             * for private allocation by other entities
             */
};


//管理一个内存block
struct buffer_head{
    unsigned long b_state;
    void* data;                 //实际的block数据
    struct list_head lru;       //链接所有的buffer_head
    struct block_device *bd;    //该block对应的块设备，用于读写访问
    unsigned long blk_no;         //默认扇区大小==BLOCK_SZ，记录该block在磁盘上对应的block号
//    int count;                  //user using this block
// page cache相关字段
//    struct buffer_head *b_this_page;/* circular list of page's buffers */
//    struct page* pg;            //该block存放于该page
//    unsigned long page_offset;  //该block在page中的offset
};


extern void init_fs(void);
extern void print_root_bdev(void);
extern struct buffer_head* get_blk(struct block_device *bd, unsigned long blk_no);


#endif
