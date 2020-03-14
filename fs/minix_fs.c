#include "minix_fs.h"
#include "fs.h"
#include "lib.h"
#include "printf.h"
#include "list.h"

struct inode_operations minix_inode_operations;


//根据inode号得到对应inode结构在所在块内的偏移，字节为单位
#define inode_offset(inode)    ((inode-1)%I_PER_BLK * I_SZ)

//根据inode号得到对应inode结构所在的块号，inode号从1开始计数
static inline unsigned long ino_to_blk_no(struct minix_sb_info *m_sbi, unsigned long inode_no)
{
    unsigned long blk_no;

    blk_no = (inode_no-1)/I_PER_BLK;
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
    INIT_LIST_HEAD(&sb->s_inodes);
    
    init_minix_bitmap(sb);

    return 0;
}


//根据inode号得到一个minix格式设备的通用inode结构
struct inode * minix_iget(struct super_block *sb, unsigned long ino)
{
    unsigned long blk_no;
    struct buffer_head *bh;
    struct minix_inode_info *m_mi;

    if(ino==0)
        return NULL;

    m_mi = (struct minix_inode_info*)kmalloc(sizeof(struct minix_inode_info));

    blk_no = ino_to_blk_no( (struct minix_sb_info*)sb->s_fs_info, ino);

    bh = get_blk(sb->bd, blk_no);

    memcpy( &m_mi->d_inode, bh->data + inode_offset(ino), sizeof(struct minix_inode) );

    m_mi->vfs_inode.i_op = &minix_inode_operations;
    m_mi->vfs_inode.sb = sb;
    list_add( &(m_mi->vfs_inode.list), &sb->s_inodes);

    return &m_mi->vfs_inode;
}


//根据偏移找到inode对应的block
struct buffer_head* inode_get_blk(struct inode *inode, unsigned long offset)
{
    unsigned long blk_offset;
    unsigned long blk_no;
    unsigned long *addr;
    struct minix_inode *d_inode = &minix_i(inode)->d_inode;
    struct block_device *bd = inode->sb->bd;


    //先计算是第多少个block
    blk_offset = offset/BLOCK_SZ;

    if( blk_offset < 7){    //直接块
        blk_no = d_inode->i_zone[blk_offset];
    }else if( blk_offset < 7 + 512){    //一次间接块，512 = BLOCK_SZ / BLK_NO_SZ, 
        addr = get_blk(bd, d_inode->i_zone[7])->data + BLK_NO_SZ * (blk_offset - 7);
        blk_no = *addr;
    }else if( blk_offset < 7 + 512 + 512 * 512 ){   //二次间接块
        addr = get_blk(bd, d_inode->i_zone[8])->data + BLK_NO_SZ * (blk_offset - 7 - 512)/512;
        blk_no = *addr;   
    }else{  //不支持的大小
        return NULL;
    }
    return get_blk(bd, blk_no);
}


//从当前目录的inode中查找名为name的文件或路径文件
static struct inode* minix_lookup(struct inode* dir, char *name)
{
    struct minix_inode *d_inode = &minix_i(dir)->d_inode;
    unsigned long sz = d_inode->i_size;
    struct buffer_head *bh;
    unsigned long offset = 0;
    struct dir_entry *de;
    unsigned long i;

    if(!IS_DIR(d_inode->i_mode))  //在非目录中查找文件
        return NULL;

    if( strlen(name) > NAME_LEN)
        return NULL;

    for(offset = 0; offset < sz; offset+= BLOCK_SZ ){
        bh = inode_get_blk(dir, offset);
        de = bh->data;

        for(i=0;i<DIR_PER_BLK;i++){
            de++;
            if( strcmp(name, de->name)==0)
                return minix_iget( dir->sb, de->inode);
        }; 
    }

    return NULL;
}


struct inode_operations minix_inode_operations = {
    .lookup = &minix_lookup ,

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
    struct minix_inode *d_inode;
    int i=0;

    d_inode = &(minix_i(minix_iget(bd->sb, inode_no))->d_inode);
    
    printf("i_mode: %#x\n", d_inode->i_mode);
    printf("i_size: %d\n", d_inode->i_size);

    for(i=0; i<9; i++){
        printf("i_zone[%d]: %d\n", i, d_inode->i_zone[i]);
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

    bh = inode_get_blk( minix_iget(bd->sb, 2), 0x0);
    print_block(bh);

    inode = minix_iget(bd->sb, 1);
    printf("==========%d=========\n", minix_i( inode->i_op->lookup( inode, "user_code.bin"))->d_inode.i_size);

}



