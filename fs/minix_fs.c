#include "minix_fs.h"
#include "fs.h"
#include "lib.h"
#include "printf.h"
#include "list.h"

static const struct inode_operations minix_inode_operations;
static const struct inode_operations minix_dir_inode_operations;
static const struct super_operations minix_sops;

//根据inode号得到对应inode结构在所在块内的偏移，字节为单位
#define inode_offset(inode)    ((inode-1)%MINIX_INODES_PER_BLOCK * I_SZ)

//根据inode号得到对应inode结构所在的块号，inode号从1开始计数
static inline unsigned long ino_to_blk_no(struct minix_sb_info *m_sbi, unsigned long inode_no)
{
    unsigned long blk_no;

    blk_no = (inode_no-1)/MINIX_INODES_PER_BLOCK;
    return 2 + m_sbi->s_imap_blocks + m_sbi->s_zmap_blocks + blk_no;
}


static int init_minix_bitmap(struct super_block *sb)
{
    int i = 0;
    unsigned long imap_blks, zmap_blks;
    struct minix_sb_info* m_sbi = sb->s_fs_info;


    imap_blks = m_sbi->s_imap_blocks;
    zmap_blks = m_sbi->s_zmap_blocks;

    m_sbi->s_imap = (struct buffer_head**)kmalloc(sizeof(struct buffer_head) * imap_blks);
    m_sbi->s_zmap = (struct buffer_head**)kmalloc(sizeof(struct buffer_head) * zmap_blks);

    for( i = 0; i < imap_blks; i++ ){
        m_sbi->s_imap[i] = get_blk(sb->bd,  MINIX_SB_OFFSET + 1 + i);
    }

    for( i=0; i < zmap_blks; i++){
        m_sbi->s_zmap[i] = get_blk(sb->bd,  MINIX_SB_OFFSET + imap_blks + 1 + i);
    }

    return 0;
}


//填充一个minix格式的块设备的super block结构
int minix_fill_super(struct super_block *sb)
{
    struct minix_sb_info *m_sbi = NULL;
    struct buffer_head *bh = NULL;
    struct minix_super_block *d_sb = NULL;

    m_sbi = (struct minix_sb_info*)kmalloc(sizeof(struct minix_sb_info));
    bh = get_blk(sb->bd, MINIX_SB_OFFSET);
    d_sb = (struct minix_super_block*)(bh->data);

    m_sbi->s_ms             = d_sb;
    m_sbi->s_ninodes        = d_sb->s_ninodes;
    m_sbi->s_nzones         = d_sb->s_nzones;
    m_sbi->s_imap_blocks    = d_sb->s_imap_blocks;
    m_sbi->s_zmap_blocks    = d_sb->s_zmap_blocks;
    m_sbi->s_firstdatazone  = d_sb->s_firstdatazone;
    m_sbi->s_log_zone_size  = d_sb->s_log_zone_size;
    m_sbi->s_max_size       = d_sb->s_max_size;
    m_sbi->s_sbh            = bh;

    sb->s_fs_info = m_sbi;
    sb->s_op = &minix_sops;
    INIT_LIST_HEAD(&sb->s_inodes);
    
    init_minix_bitmap(sb);

    return 0;
}


static struct minix_inode * minix_raw_inode(struct super_block *sb, unsigned long ino, struct buffer_head **bh)
{
    int block;
    struct minix_sb_info *sbi = minix_sb(sb);
    struct minix_inode *p; 

    if (!ino || ino > sbi->s_ninodes) {
        printf("Bad inode number: %lu is out of range\n", ino);
        return NULL;
    }
    ino--;
    block = 2 + sbi->s_imap_blocks + sbi->s_zmap_blocks + ino / MINIX_INODES_PER_BLOCK;
    *bh = bread(sb->bd, block);
    if (!*bh) {
        printf("Unable to read inode block\n");
        return NULL;
    }
    p = (void *)(*bh)->data;
    return p + ino % MINIX_INODES_PER_BLOCK;
}


//根据inode号得到一个minix格式设备的通用inode结构
struct inode * minix_iget(struct super_block *sb, unsigned long ino)
{
    struct buffer_head *bh;
    struct minix_inode_info *m_mi;
    struct inode* inode;
    struct minix_inode *d_inode;

    if(ino==0)
        return NULL;

    inode = get_inode(sb, ino);

    if(inode->i_state & I_NEW){
        m_mi = list_entry(inode, struct minix_inode_info, vfs_inode);

        d_inode = minix_raw_inode( sb, ino, &bh );

        memcpy( m_mi->i_data, d_inode->i_zone, 9 * 2 );

        inode->i_mode = d_inode->i_mode;
        inode->i_uid = d_inode->i_uid;
        inode->i_gid = d_inode->i_gid;
        inode->i_size = d_inode->i_size;
        inode->i_atime = d_inode->i_time;
        inode->i_ctime = d_inode->i_time;
        inode->i_mtime = d_inode->i_time;

        if(S_ISDIR(inode->i_mode)){
            inode->i_op = &minix_dir_inode_operations;
        }else if(S_ISREG(inode->i_mode)){
            inode->i_op = &minix_inode_operations;
        }else{
           // inode->i_op = &minix_inode_operations;
        }


        inode->i_state &= ~I_NEW;
    }

    return inode;
}


//根据偏移找到inode对应的block
struct buffer_head* inode_get_blk(struct inode *inode, unsigned long offset)
{
    unsigned long blk_offset;
    unsigned long blk_no;
    unsigned long *addr;
    struct minix_inode_info *m_ii = minix_i(inode);
    struct block_device *bd = inode->sb->bd;


    //先计算是第多少个block
    blk_offset = offset/BLOCK_SZ;

    if( blk_offset < 7){    //直接块
        blk_no = m_ii->i_data[blk_offset];
    }else if( blk_offset < 7 + 512){    //一次间接块，512 = BLOCK_SZ / BLK_NO_SZ, 
        addr = get_blk(bd, m_ii->i_data[7])->data + BLK_NO_SZ * (blk_offset - 7);
        blk_no = *addr;
    }else if( blk_offset < 7 + 512 + 512 * 512 ){   //二次间接块
        addr = get_blk(bd, m_ii->i_data[8])->data + BLK_NO_SZ * (blk_offset - 7 - 512)/512;
        blk_no = *addr;
    }else{  //不支持的大小
        return NULL;
    }
    return get_blk(bd, blk_no);
}


//从当前目录的inode中查找名为name的文件或路径文件
static struct inode* minix_lookup(struct inode* dir, char *name)
{
    unsigned long sz = dir->i_size;
    struct buffer_head *bh;
    unsigned long offset = 0;
    struct dir_entry *de;
    unsigned long i;

    if(!S_ISDIR(dir->i_mode))  //在非目录中查找文件
        return NULL;
    if( strlen(name) > NAME_LEN)
        return NULL;

    for(offset = 0; offset < sz; offset+= BLOCK_SZ ){
        bh = inode_get_blk(dir, offset);
        de = bh->data;

        for(i=0;i<DIR_PER_BLK;i++){
            if( strcmp(name, de->name)==0){
                return minix_iget( dir->sb, de->inode);
            }
            de++;
        }; 
    }

    return NULL;
}


static int minix_read(struct inode* inode, char* buf, int pos , int count)
{
    int left,chars,nr;
    char *p; 
    struct buffer_head * bh; 

    left = count;
    while (left) {
        bh = inode_get_blk(inode, pos);
        if(bh == NULL)
            return -1; 
        nr = pos % BLOCK_SZ;
        chars = min( BLOCK_SZ-nr , left);
        pos += chars;
        left -= chars;

        p = nr + bh->data;
        while (chars-->0){
            *buf = *p;
            buf++;
            p++;
        }
        mark_buffer_dirty(bh);
        brelse(bh);
    }
    //inode->i_atime = CURRENT_TIME;
    return (count-left)?(count-left):-1;
}

static const struct inode_operations minix_inode_operations = {
    .read = &minix_read,
};


static const struct inode_operations minix_dir_inode_operations = {
    .lookup = &minix_lookup ,
};

/*
const struct inode_operations minix_dir_inode_operations = { 
    .create     = minix_create,
    .lookup     = minix_lookup,
    .link       = minix_link,
    .unlink     = minix_unlink,
    .symlink    = minix_symlink,
    .mkdir      = minix_mkdir,
    .rmdir      = minix_rmdir,
    .mknod      = minix_mknod,
    .rename     = minix_rename,
    .getattr    = minix_getattr,
};
*/


static struct inode *minix_alloc_inode(struct super_block *sb)
{
    struct minix_inode_info *ei;
    ei = (struct minix_inode_info *)kmalloc(sizeof(struct minix_inode_info));
    if (!ei)
        return NULL;
    return &ei->vfs_inode;
}

static void minix_destroy_inode(struct inode* inode)
{
    if(inode==NULL)
        return ;

    kfree(minix_i(inode));
}

static int minix_write_inode(struct inode *inode)
{
    struct buffer_head * bh; 
    struct minix_inode * raw_inode;
    struct minix_inode_info *minix_inode = minix_i(inode);
    int i;

    raw_inode = minix_raw_inode(inode->sb, inode->i_ino, &bh);
    if (!raw_inode)
        return 0;
    raw_inode->i_mode = inode->i_mode;
    raw_inode->i_uid = inode->i_uid;
    raw_inode->i_gid = inode->i_gid;
    raw_inode->i_nlinks = inode->i_nlink;
    raw_inode->i_size = inode->i_size;
    raw_inode->i_time = inode->i_mtime;
    
    for (i = 0; i < 9; i++)
        raw_inode->i_zone[i] = minix_inode->i_data[i];

    mark_buffer_dirty(bh);

    brelse(bh);

    return 0;
}


static void minix_put_super(struct super_block *sb)
{
    int i;
    struct minix_sb_info *sbi = minix_sb(sb);

    for (i = 0; i < sbi->s_imap_blocks; i++)
        brelse(sbi->s_imap[i]);
    for (i = 0; i < sbi->s_zmap_blocks; i++)
        brelse(sbi->s_zmap[i]);
    brelse (sbi->s_sbh);
    kfree(sbi->s_imap);
    kfree(sbi->s_zmap);
    sb->s_fs_info = NULL;
    kfree(sbi);
}


static const struct super_operations minix_sops = { 
    .alloc_inode    = minix_alloc_inode,
    .destroy_inode  = minix_destroy_inode,
    .write_inode    = minix_write_inode,
//    .evict_inode    = minix_evict_inode,
    .put_super  = minix_put_super,
//    .statfs     = minix_statfs,
//    .remount_fs = minix_remount,
};


void print_block(struct buffer_head* bh)
{
    int i ;
    void *addr;
    void *addr0;

    addr0 = bh->data ;

    for(i=0; i<BLOCK_SZ; i+=sizeof(unsigned int)){
        addr = (unsigned int*)(addr0+i) ;
        printf("addr: %#lx, val: %#lx; ", (unsigned int*)(addr-addr0), *(unsigned int*)addr);
        if(i%20==0 && i!=0)
            printf("\n");
    }
    printf("\n");
}


void print_minix_inode(struct block_device *bd, unsigned long inode_no)
{
    struct inode *inode;
    struct minix_inode_info *m_ii;
    int i=0;

    inode = minix_iget(bd->sb, inode_no);
    
    printf("i_mode: %#x\n", inode->i_mode);
    printf("i_size: %d\n", inode->i_size);

    m_ii = minix_i(inode);

    for(i=0; i<9; i++){
        printf("i_data[%d]: %d\n", i, m_ii->i_data[i]);
    }
}


void print_minix_sb(struct super_block *sb)
{
    struct minix_sb_info *m_sbi = sb->s_fs_info;

    printf("ninodes: %d, nzones: %d, imap_blocks: %d, zmap_blocks: %d\n", 
        m_sbi->s_ninodes, m_sbi->s_nzones, m_sbi->s_imap_blocks, m_sbi->s_zmap_blocks);
    printf("s_firstdatazone: %d, s_max_size: %d\n", 
        m_sbi->s_firstdatazone, m_sbi->s_max_size);
}


//打印一个目录
void print_dir_entry(struct dir_entry* dir)
{
    printf("ino: %d, file name: %s\n", dir->inode, dir->name);
}


//打印一个块中的所有目录
void print_dir_entries(struct buffer_head* bh)
{
    struct dir_entry *dir = bh->data;

    while(1){
        dir++;
        if(dir->inode==0)
            break;
        print_dir_entry(dir);
    };
}


void print_minix(struct block_device *bd)
{
    struct buffer_head *bh;
    struct super_block *sb = bd->sb;
    struct inode* inode;

    print_minix_sb(sb);
    print_dir_entries( get_blk(bd, 696) );
    print_minix_inode(bd, MINIX_ROOT_INO);
    print_minix_inode(bd, 2);
    print_minix_inode(bd, 3);
    print_dir_entries( get_blk(bd, 698) );

   // bh = inode_get_blk( minix_iget(bd->sb ,1) , 0 );
    (void)bh;
   // print_block(bh);
    inode = minix_iget(bd->sb, 1);

    inode = namei( inode, "./dir1/dir2/user_code.bin");
    if(inode)
        printf("==========ino: %d=========\n", inode->i_ino);
    else
        printf("not found\n");
}



