#ifndef _FS_H
#define _FS_H

#include "type.h"
#include "list.h"
#include "mm.h"


#define S_IFMT  00170000
#define S_IFSOCK 0140000
#define S_IFLNK  0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001


#define I_NEW       (8)

#define SEEK_SET    (0)
#define SEEK_CUR    (1)
#define SEEK_END    (2)

struct file{
    struct inode *f_inode;
    unsigned long f_count;
    int f_flags;
    unsigned long f_pos;
};


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
    int (*read)(struct inode* inode, char* buf, int pos , int count);
    int (*write)(struct inode* inode, char* buf, int pos , int count);
};

struct inode{
    unsigned long i_mode;
    unsigned long i_uid;
    unsigned long i_gid;
    unsigned long i_size;
    unsigned long i_atime;
    unsigned long i_mtime;
    unsigned long i_ctime;
    unsigned long i_state;
    unsigned long i_nlink;

    unsigned long i_ino;
    struct list_head list;       //用于链接同一super_block的所有inode
    const struct inode_operations *i_op;
    struct super_block *sb;
    unsigned long i_count;
//    void *i_private;            //fs or device private pointer ,指向具体文件系统的inode
};


struct super_operations{
    struct inode *(*alloc_inode)(struct super_block *sb);
    void (*destroy_inode)(struct inode *);
    int (*write_inode) (struct inode * );
    void (*put_super) (struct super_block *);
};


struct super_block{
    const struct super_operations *s_op;
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

#define mark_buffer_dirty(bh)   (bh->b_state = BH_Dirty)

extern void init_fs(void);
extern void print_root_bdev(void);
extern struct buffer_head* get_blk(struct block_device *bd, unsigned long blk_no);
extern struct buffer_head* bread( struct block_device *bd, unsigned long blk_no);
extern int brelse(struct buffer_head * bh);
extern struct inode* namei(struct inode *cur_dir, const char *path);
extern struct inode* get_inode(struct super_block *sb, unsigned long ino);

extern int file_open(const char * filename, int flag, int mode);
extern int file_read(struct file * filp, char * buf, int count);
extern int file_write(struct file * filp, char * buf, int count);
extern int file_close(int fd);
extern int file_lseek(struct file *filp, int offset, int whence);
#endif
