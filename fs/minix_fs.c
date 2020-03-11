#include "minix_fs.h"
#include "fs.h"
#include "lib.h"
#include "printf.h"

//读取外存上的minix系统的超级块，填充并返回minix超级块的内存结构
struct minix_sb_info* alloc_minix_sb(struct block_device *bd)
{
    struct minix_sb_info *m_sbi = NULL;
    void* block = kmalloc(BLOCK_SZ);
    struct minix_super_block *sb = NULL;

    m_sbi = (struct minix_sb_info*)kmalloc(sizeof(struct minix_sb_info));

    bd->genhd->blkdev_op->direct_read(SB_OFFSET, block);

    sb = (struct minix_super_block*)block;

    m_sbi->s_ms             = sb;
    m_sbi->s_ninodes        = sb->s_ninodes;
    m_sbi->s_nzones         = sb->s_nzones;
    m_sbi->s_imap_blocks    = sb->s_imap_blocks;
    m_sbi->s_zmap_blocks    = sb->s_zmap_blocks;
    m_sbi->s_firstdatazone  = sb->s_firstdatazone;
    m_sbi->s_log_zone_size  = sb->s_log_zone_size;
    m_sbi->s_max_size       = sb->s_max_size;


    return m_sbi;
}


void print_minix_sb(struct minix_sb_info *m_sbi)
{
    printf("ninodes: %d, nzones: %d, imap_blocks: %d, zmap_blocks: %d\n", 
        m_sbi->s_ninodes, m_sbi->s_nzones, m_sbi->s_imap_blocks, m_sbi->s_zmap_blocks);
    printf("s_firstdatazone: %d, s_max_size: %d\n", 
        m_sbi->s_firstdatazone, m_sbi->s_max_size);


    

}

