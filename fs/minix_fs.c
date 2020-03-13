#include "minix_fs.h"
#include "fs.h"
#include "lib.h"
#include "printf.h"

static int init_minix_bitmap(struct minix_sb_info* m_sbi)
{
    int i = 0;
    unsigned long imap_blks, zmap_blks;

    imap_blks = m_sbi->s_imap_blocks;
    zmap_blks = m_sbi->s_zmap_blocks;

    m_sbi->s_imap = (struct buffer_head**)kmalloc(sizeof(struct buffer_head) * imap_blks);
    m_sbi->s_zmap = (struct buffer_head**)kmalloc(sizeof(struct buffer_head) * zmap_blks);

    for( i = 0; i < imap_blks; i++ ){
        m_sbi->s_imap[i] = get_blk( SB_OFFSET + 1 + i);
    }

    for( i=0; i < zmap_blks; i++){
        m_sbi->s_zmap[i] = get_blk( SB_OFFSET + imap_blks + 1 + i);
    }

    return 0;
}


//读取外存上的minix系统的超级块，填充并返回minix超级块的内存结构
struct minix_sb_info* alloc_minix_sb(struct block_device *bd)
{
    struct minix_sb_info *m_sbi = NULL;
    struct buffer_head *bh = NULL;
    struct minix_super_block *sb = NULL;

    m_sbi = (struct minix_sb_info*)kmalloc(sizeof(struct minix_sb_info));
    bh = get_blk(SB_OFFSET);
    sb = (struct minix_super_block*)(bh->data);

    m_sbi->s_ms             = sb;
    m_sbi->s_ninodes        = sb->s_ninodes;
    m_sbi->s_nzones         = sb->s_nzones;
    m_sbi->s_imap_blocks    = sb->s_imap_blocks;
    m_sbi->s_zmap_blocks    = sb->s_zmap_blocks;
    m_sbi->s_firstdatazone  = sb->s_firstdatazone;
    m_sbi->s_log_zone_size  = sb->s_log_zone_size;
    m_sbi->s_max_size       = sb->s_max_size;
    m_sbi->s_sbh            = bh;

    init_minix_bitmap(m_sbi);

    return m_sbi;
}


//得到一个minix格式设备的根目录
struct minix_inode* minix_get_inode(struct block_device *bd, unsigned long inode_no)
{
    struct minix_sb_info *m_sbi;
    unsigned long blk_no;
    struct buffer_head *bh;

    if(inode_no==0)
        return NULL;

    m_sbi = (struct minix_sb_info *)bd->sb->s_private;

    blk_no = inode_to_blk(m_sbi, inode_no);

    bh = get_blk(blk_no);

    return (struct minix_inode*)(bh->data+inode_offset(inode_no));
}


//根据inode的偏移找到对应的block
struct buffer_head* inode_offset_to_bh(struct minix_inode *d_inode, unsigned long offset)
{
    unsigned long blk_offset;
    unsigned long blk_no;
    unsigned long *addr;

    //先计算是第多少个block
    blk_offset = offset/BLOCK_SZ;

    if( blk_offset < 7){    //直接块
        blk_no = d_inode->i_zone[blk_offset];
    }else if( blk_offset < 7 + 512){    //一次间接块，512 = BLOCK_SZ / BLK_NO_SZ, 
        addr = get_blk(d_inode->i_zone[7])->data + BLK_NO_SZ * (blk_offset - 7);
        blk_no = *addr;
    }else if( blk_offset < 7 + 512 + 512 * 512 ){   //二次间接块
        addr = get_blk(d_inode->i_zone[8])->data + BLK_NO_SZ * (blk_offset - 7 - 512)/512;
        blk_no = *addr;   
    }else{  //不支持的大小
        return NULL;
    }
    return get_blk(blk_no);
}

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

    d_inode = minix_get_inode(bd, inode_no);
    
    printf("i_mode: %#x\n", d_inode->i_mode);
    printf("i_size: %d\n", d_inode->i_size);

    for(i=0; i<9; i++){
        printf("i_zone[%d]: %d\n", i, d_inode->i_zone[i]);
    }
}


void print_minix_sb(struct minix_sb_info *m_sbi)
{
    printf("ninodes: %d, nzones: %d, imap_blocks: %d, zmap_blocks: %d\n", 
        m_sbi->s_ninodes, m_sbi->s_nzones, m_sbi->s_imap_blocks, m_sbi->s_zmap_blocks);
    printf("s_firstdatazone: %d, s_max_size: %d\n", 
        m_sbi->s_firstdatazone, m_sbi->s_max_size);
}

//打印一个目录
void print_dir_entry(struct dir_entry* dir)
{
    printf("file name: %s\t inode_no in disk: %d\n", dir->name, dir->inode);
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
    struct minix_inode *d_inode;


    print_minix_sb(bd->sb->s_private);
    print_minix_inode(bd, ROOT_INODE);
    print_dir_entries( get_blk(696) );
    print_minix_inode(bd, 3);

    d_inode = minix_get_inode(bd, 3);
    bh = inode_offset_to_bh( d_inode, 0x800);
    print_block(bh);
}








