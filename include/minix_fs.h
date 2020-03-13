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
    zone bitmap 最低位置为1，从第一位才开始使用，因此data zone  从1开始计数
*/


#define SB_OFFSET   (1)
#define ROOT_INODE  (1)     //inode从1开始计数

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
#define DIR_BIT         (14)
#define FILE_BIT        (15)

#define IS_DIR(mode)    ((1<<DIR_BIT)&mode)
#define IS_FILE(mode)   ((1<<FILE_BIT)&mode)


/*
 * This is the original minix inode layout on disk.
 * Note the 8-bit gid and atime and ctime.
 */
//32Byte
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
// 目录被实现为一种特殊的文件, 目录的数据由一个或多个dir_entry结构组成
// 32B
struct dir_entry {
    __u16 inode;            //文件的实际inode号
    char  name[NAME_LEN];   //文件的名字
};

#define INODE_NO_SZ             (2)                     //磁盘上存储的inode号的大小
#define NR_INODE_NO_PER_BLOCK   (BLOCK_SZ/INODE_NO_SZ)  //512
#define BLK_NO_SZ               (2)                     //磁盘上存储的blk号的大小
#define NR_BLK_NO_PER_BLOCK     (BLOCK_SZ/BLK_NO_SZ)    //512


/*
 * minix super-block data in memory
 */
struct minix_sb_info {
    unsigned long s_ninodes;
    unsigned long s_nzones;
    unsigned long s_imap_blocks;
    unsigned long s_zmap_blocks;
    unsigned long s_firstdatazone;
    unsigned long s_log_zone_size;
    unsigned long s_max_size;
//    int s_dirsize;    32B
//    int s_namelen;    30B
    struct buffer_head ** s_imap;
    struct buffer_head ** s_zmap;
    struct buffer_head * s_sbh;
    struct minix_super_block * s_ms;
//    unsigned short s_mount_state;
//    unsigned short s_version;
};


#define I_SZ        (sizeof(struct minix_inode))    //32Byte
#define I_PER_BLK   (BLOCK_SZ/I_SZ)                 //1024/32==32

//根据inode号得到对应inode结构所在的块号，inode号从1开始计数
static inline unsigned long inode_to_blk(struct minix_sb_info *m_sbi, unsigned long inode_no)
{
    unsigned long blk_no;

    blk_no = (inode_no-1)/I_PER_BLK;
    return 2 + m_sbi->s_imap_blocks + m_sbi->s_zmap_blocks + blk_no;
}

//根据inode号得到对应inode结构在所在块内的偏移，字节为单位
#define inode_offset(inode)    ((inode-1)%I_PER_BLK * I_SZ)


extern struct minix_sb_info* alloc_minix_sb(struct block_device *);
extern void print_minix_sb(struct minix_sb_info *m_sb);
extern void print_minix(struct block_device *bd);

#endif
