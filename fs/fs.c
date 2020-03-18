#include "fs.h"
#include "minix_fs.h"
#include "lib.h"
#include "printf.h"
#include "board.h"
#include "list.h"
#include "task.h"

//系统的根设备，根设备的根路径对应系统的"/"
struct block_device *root_bdev = NULL;
struct inode *root_inode = NULL;
//定义所有buffer_head的LRU链表的头节点
LIST_HEAD(bh_head);


static int pflash_read(sector_t, struct buffer_head *bh);
static int pflash_write(sector_t, struct buffer_head *bh);


static struct blkdev_operations pflash_ops = {
    .read = pflash_read,
    .write = pflash_write,
};


static struct gendisk pflash = {
    .sector_sz =  BLOCK_SZ,
    .nr_sector = PFLASH1_SIZE / BLOCK_SZ,
    .blkdev_op = &pflash_ops,
    .diskname = "pflash1",
};


void init_fs(void)
{
    root_bdev = (struct block_device *)kmalloc(sizeof(struct block_device));
    root_bdev->genhd = &pflash;
    root_bdev->nr_blk = root_bdev->genhd->sector_sz * root_bdev->genhd->nr_sector / BLOCK_SZ;
    root_bdev->sb = (struct super_block*)kmalloc(sizeof(struct super_block));
    root_bdev->sb->bd = root_bdev;
    minix_fill_super(root_bdev->sb);

    root_inode = minix_iget(root_bdev->sb, MINIX_ROOT_INO);

}


static int pflash_read(sector_t sect, struct buffer_head *bh)
{
    memcpy( bh->data, (void*)(PFLASH1_BASE + sect * BLOCK_SZ), BLOCK_SZ);
    return 0;
}


#define pflash1_addr(m)     (unsigned char*)(PFLASH1_BASE + m)
#define CMD_WR              (0x10)
#define CMD_WR_END          (0xff)
#define CMD_STATE_REG       (0x70)
#define SR_7                (0x80)

static inline void pflash_write_byte(unsigned long addr, unsigned char val)
{
    *pflash1_addr(addr) = CMD_WR;
    *pflash1_addr(addr) = val;
}

static int pflash_write(sector_t sect, struct buffer_head *bh)
{
    int i = 0;
    unsigned long base = sect*BLOCK_SZ;
    for(i=0; i<BLOCK_SZ; i++){
        pflash_write_byte( base + i , *(unsigned char*)(bh->data + i) );
    }
    while(!(*pflash1_addr(CMD_STATE_REG) & SR_7));      //wait for write complete
    *pflash1_addr(0) = CMD_WR_END;                      //any addr is ok, enter read mode
    return i;
}


//为指定的块分配一个buffer_head和block
//现在的block从kmallc中分配，如果实现page cache，
//可以两者结合，从page cache层来管理
//利用page cache，读写一个文件应以页为单位，再利用buffer head来处理对设备的写入
struct buffer_head* bread( struct block_device *bd, unsigned long blk_no)
{
    struct buffer_head *bh;

    bh = (struct buffer_head*)kmalloc(sizeof(struct buffer_head));
    bh->data = kmalloc(BLOCK_SZ);

    bh->bd = bd;
    bh->bd->genhd->blkdev_op->read( blk_no, bh );
    bh->b_state = BH_Uptodate;
    bh->blk_no = blk_no;

    list_add( &bh->lru, &bh_head);     //LRU链表，新节点加到最前面

    return bh;
}


// 为了方便的查找到指定块号对应的buffer_head，可以用hash链表来加速查找(暂不实现)
struct buffer_head* get_blk(struct block_device *bd, unsigned long blk_no)
{
    struct list_head *pos;
    struct buffer_head *bh;

    list_for_each(pos, &bh_head){
        bh = list_entry(pos, struct buffer_head, lru);
        if( (bh->blk_no == blk_no) && (bh->bd == bd) )
            return bh;
    }
    //目前不存在，新建
    bh = bread(bd, blk_no);

    return bh;
}


//先同步磁盘，再分配释放block和buffer head
int brelse(struct buffer_head * bh)
{
    if(bh->b_state == BH_Dirty){
        bh->bd->genhd->blkdev_op->write( bh->blk_no, bh ); 
    }

    kfree(bh->data);
    list_del(&bh->lru);
    kfree(bh);

    return 0;
}


// path = "///asd.c/sad"
// offset = 3 <= "///"
// return = 5 <= "asd.c"
static int path_len( const char *path, unsigned long *offset)
{
    int i = 0;
    
    *offset = 0;
    if(path==NULL || *path == '\0')
        return 0;

    while(*path=='/'){
        path++;
        (*offset)++;
    }

    while(*path!= '/' && *path!='\0'){
        path++;
        i++;
    }
    return i;
}

//根据当前路径的inode及文件路径名找到文件inode
struct inode* namei(struct inode *cur_dir, const char *path)
{
    struct inode* dir;
    int length ;
    char *name;
    unsigned long offset;

    if(path==NULL){
        return NULL;
    }else if(*path == '/'){   //从根目录开始计算
        dir = root_inode;
    }else if(cur_dir==NULL){
        return NULL;
    }else{
        dir = cur_dir;
    }

    if(*path == '.' && *(path+1)=='/'){
        path+=2;
    }

    name = (char*)kmalloc(strlen(path) + sizeof('\0'));

    while( (length=path_len(path, &offset))!=0 ){
        path += offset;
        memcpy(name, (void*)path, length);
        path += length;
        *(name+length) = '\0';
        
        dir = dir->i_op->lookup(dir, name);
        if(dir==NULL)
            break;
    }

    kfree(name);

    return dir;
}


static struct inode* get_new_inode(struct super_block *sb, unsigned long ino)
{
    struct inode *inode;

    inode = sb->s_op->alloc_inode(sb);

    inode->sb = sb;
    inode->i_ino = ino;
    inode->i_op = NULL;
    inode->i_state = I_NEW;
    list_add(&inode->list, &sb->s_inodes);

    return inode;
}


struct inode* get_inode(struct super_block *sb, unsigned long ino)
{
    struct inode* inode;    
    struct list_head *pos;

    list_for_each(pos, &sb->s_inodes){
        inode = list_entry(pos, struct inode, list);
        if( (inode->i_ino == ino) && (inode->sb == sb) )
            return inode;
    }

    return get_new_inode(sb, ino);
}


void write_inode(struct inode* inode)
{
    inode->sb->s_op->write_inode(inode);
}


//free内存中的inode对象并写回磁盘，inode对象的数据存放在buffer中，待写回
void generic_drop_inode(struct inode *inode)
{
    write_inode(inode);

    inode->sb->s_op->destroy_inode(inode);
}


struct file file_tbl[NR_FILE];


int file_open(const char * filename, int flag, int mode)
{
    struct inode * inode;
    struct file * f;
    int i,fd;

    (void)mode;

    for(fd=0 ; fd<NR_OPEN ; fd++)
        if (!current->filp[fd])
            break;
    if (fd>=NR_OPEN)
        return -1;

    f = file_tbl;
    for (i=0 ; i<NR_FILE ; i++,f++)
        if (!f->f_count) break;
    if (i>=NR_FILE)
        return -2;

    current->filp[fd]=f;
    
    inode = namei( NULL, filename);
    if(inode==NULL) {
        current->filp[fd]=NULL;
        f->f_count=0;
        return -3;
    } 

//    f->f_mode = inode->i_mode;
    f->f_flags = flag;
    f->f_count = 1;
    f->f_inode = inode;
    f->f_pos = 0;
    
    return (fd);
}


int file_read(struct file * filp, char * buf, int count)
{
    int ret;
    struct inode *inode;

    if (count<=0)
        return 0;

    inode = filp->f_inode;

    ret = inode->i_op->read(inode, buf, filp->f_pos, count);
    if(ret>0)
        filp->f_pos+=ret;

    return ret?ret:-1;
}


void print_root_bdev(void)
{
    print_minix(root_bdev);
}

