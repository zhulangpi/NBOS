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
// 1. inode offset -> block offset
// 2. block -> 磁盘sector

// 每个block用buffer_head来描述
// buffer_head记录该block对应于inode的offset，同一inode的buffer_head根据offset顺序链接
// 解决inode offset -> block offset的转换

// buffer_head记录磁盘扇区号
// 解决block -> sector 的转换


struct block_device;
struct buffer_head;

struct inode{
    struct list_head bh_list;       //用于链接该inode的所有buffer_head
//    struct inode_operations *i_op;
    struct block_device *bd;
    void *i_private;
};


struct super_block{
//    struct super_block_operations *s_op;
    struct block_device *bd;
    void *s_private;
};


//block必须>=sector
struct blkdev_operations{
    int (*read)(sector_t, struct buffer_head *bh);    //读一个完整block
    int (*write)(sector_t, struct buffer_head *bh);   //写一个完整block
    int (*direct_read) (sector_t sec, unsigned long *addr);     //直接读一个block
    int (*direct_write) (sector_t sec, unsigned long *addr);    //直接写一个block
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
    struct list_head inode;   //该块设备的所有inode的链表
};


//管理一个内存block
struct buffer_head{
    struct list_head list;      //链接同一inode的buffer_head
    unsigned long inode_offset; //记录该block首地址对应在inode中的offset
    sector_t sector_no;         //默认扇区大小==BLOCK_SZ，记录该block对应磁盘上的sector号
    struct page* pg;            //该block存放于该page
    unsigned long page_offset;  //该block在page中的offset
    struct block_device *bd;    //该block对应的块设备，用于读写访问
};


extern void init_fs(void);

#endif
