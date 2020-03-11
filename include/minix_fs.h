#ifndef _MINIX_FS_H
#define _MINIX_FS_H

#include "type.h"
#include "fs.h"

#define BLOCK_SZ  (1024)


/*
Minix v1 file system structure
zone:  0           1              2             2 + imap_blk        ...         ...
+----------------------------------------------------------------------------------------+
| bootsector | superblock | inode bitmap ... | zone bitmap ... | inodes zone | data zone |
+----------------------------------------------------------------------------------------+
1 zone = 2 block(physical) = 1024 byte
其中，
    inode bitmap最低位置为1，从第一位才开始使用，因此inodes zone从1开始计数
*/


#define SB_OFFSET  (1)

/*
 * minix super-block data on disk
 */
struct minix_super_block {
    __u16 s_ninodes;        // number of inodes
    __u16 s_nzones;         // number of zones
    __u16 s_imap_blocks;    // i 节点位图 占用块的数目
    __u16 s_zmap_blocks;    // 数据块位图 占用的块的数目
    __u16 s_firstdatazone;  // 第一个 数据块 的块号
    __u16 s_log_zone_size;  // 一个虚拟块的大小 = 1024 << log_zone_size
    __u32 s_max_size;       // 能存放的最大文件大小(以 byte 计数)
    __u16 s_magic;          // 0x138f
    __u16 s_state;
    __u32 s_zones;
};


/*
inode mode:
    bit8-bit0: 宿主(rwx)-组员(rwx)-其他(rwx)
    bit9: 执行时设置uid
    bit10: 执行时设置gid
    bit11: 不清楚
    bit12: FIFO 文件
    bit13: 字符设备文件
    bit14: 目录文件
    bit15: 常规文件
    当bit13, bit14 都设置时，代表块设备文件
*/

/*
 * This is the original minix inode layout on disk.
 * Note the 8-bit gid and atime and ctime.
 */
struct minix_inode {
    __u16 i_mode;
    __u16 i_uid;    //文件属主的用户ID，为0 表示为root
    __u32 i_size;   //byte as unit
    __u32 i_time;   //自从 1970.1.1 以来的秒数，可以用date 命令加以转换，例如: date -d @`printf "%d\n" 0x57bebc17`
    __u8  i_gid;    //文件属主属于的组
    __u8  i_nlinks; //该节点被多少个目录所链接
    __u16 i_zone[9];//7个直接块，一个间接块，一个两级间接块
};


#define NAME_LEN    (30)
//目录被实现为一种特殊的文件, 目录的数据由一个或多个dir_entry结构组成
struct dir_entry {
    unsigned short inode;   //文件的实际inode号
    char name[NAME_LEN];    //文件的名字
};


extern struct minix_super_block* alloc_minix_sb(struct block_device *);


#endif
