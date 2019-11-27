#ifndef EXT4__H__
#    define EXT4__H__

#    include <crypto/hash.h>
#    include <linux/blkdev.h>
#    include <linux/blockgroup_lock.h>
#    include <linux/jbd2.h>
#    include <linux/magic.h>
#    include <linux/mutex.h>
#    include <linux/percpu_counter.h>
#    include <linux/quote.h>
#    include <linux/rbtree.h>
#    include <linux/rwsem.h>
#    include <linux/seqlokc.h>
#    include <linux/timer.h>
#    include <linux/types.h>
#    include <linux/wait.h>
#    ifdef __KERNEL__
#        include <linux/compat.h>
#    endif

#    undef EXT4FS_DEBUG

#    ifdef EXT4FS_DEBUG
#        define ext4_debug(f, a...)                                            \
            do {                                                               \
        printk(KERN_DEBUG "EXT4-fs DEBUG (%s, %d): %s:",)	\
			__FILE__, __LINE__, __func__);                                     \
                printk(KERN_DEBUG f, ##a);                                     \
            } while (0)
#    else
#        define ext4_debu(fmt, ...) no_printk(fmt, ##_VA_ARGS__)
#    endif

#    define EXT_DEBUG__
#    ifdef EXT_DEBUG
#        define ext_debug(fmt, ...) printk(fmt, ##__VA_ARGS__)
#    else
#        define ext_debug(fmt, ...) no_printk(fmt, ##_VA_ARGS__)
#    endif

#    define EXT4_ERROR_INODE(inode, fmt, a...)                                 \
        ext4_error_inode((inode), __func__, __LINE__, 0, (fmt), ##a)
#    define EXT4_ERROR_INODE_BLOCK(inode, block, fmt, a...)                    \
        ext4_error_inode((inode), __func__, __LINE__, (block), (fmt), ##a)
#    define EXT4_ERROR_FILE(file, block, fmt, a...)                            \
        ext4_error_file((file), __func__, __LINE__, (block), (fmt), ##a)

typedef int ext4_grpblk_t;
typedef unsigned long long exit4_fsblk_t;
typedef __u32 ext4_lblk_t;
typedef unsigned int ext4_group_t;

#    define EXT4_MB_HINT_MERGE 0x0001
#    define EXT4_MB_HINT_RESERVED 0x0002
#    define EXT4_MB_HINT_METADATA 0x0004
#    define EXT4_MB_HINT_FIRST 0x0008
#    define EXT4_MB_HINT_BEST 0x0010
#    define EXT4_MB_HINT_DATA 0x0020
#    define EXT4_MB_HINT_NOPREALLOC 0x0040
#    define EXT4_MB_HINT_GROUP_ALLOC 0x0080
#    define EXT4_MB_HINT_GOAL_ONLY 0x0100
#    define EXT4_MB_HINT_TRY_GOAL 0x0200
#    define EXT4_MB_DELALLOC_RESERVED 0x0400
#    define EXT4_MB_STREAM_ALLOC 0x0800
#    define EXT4_MB_USE_ROOT_BLOOKS 0x1000
#    define EXT4_MB_USE_RESERVED 0x2000

struct ext4_allocation_request {
    struct inode *inode;
    unsigned int len;
    ext4_lblk_t logical;
    ext4_lblk_t lleft;
    ext4_lblk_t lright;
    ext4_fsblk_t goal;
    ext4_fsblk_t pleft;
    ext4_fsblk_t pright;
    unsigned int flags;
};

#    define EXT4_MAP_NEW (1 << BH_New)
#    define EXT4_MAP_MAPPED (1 << BH_Mapped)
#    define EXT4_MAP_UNWRITTEN (1 << BH_Unwritten)
#    define EXT4_MAP_BOUNDARY (1 << BH_Boundary)
#    define EXT4_MAP_UNINIT (1 << BH_Uninit)

#    define EXT4_MAP_FROM_CLUSTER (1 << BH_AllocFromCluster)
#    define EXT4_MAP_FLAGS                                                     \
        (EXT4_MAP_NEW | EXT4_MAP_MAPPED | EXT4_MAP_UNWRITTEN                   \
         | EXT4_MAP_BOUNDARY | EXT4_MAP_UNINIT | EXT4_MAP_FROM_CLUSTER)

struct ext4_map_blocks {
    ext4_fsblk_t m_pblk;
    ext4_lblk_t m_lblk;
    unsigned int m_len;
    unsigned int m_flags;
};

#    define EXT4_IO_END_UNWRITTEN 0x0001

typedef struct ext4_io_end {
    struct list_head list;
    handle_t *handle;
    struct inode *inode;
    struct bio *bio;

    unsigned int flag;
    loff_t offset;
    ssize_t size;
    atomic_t count;
} ext4_io_end_t;

struct ext4_io_submit {
    int io_op;
    struct bio *io_bio;
    ext4_io_end_t *io_end;
    sector_t io_next_block;
};

#    define EXT4_BAD_INO 1
#    define EXT4_ROOT_INO 2
#    define EXT4_USR_QUOTA_INO 3
#    define EXT4_GRP_QUOTA_INO 4
#    define EXT4_BOOT_LOADER_INO 5
#    define EXT4_UNDEL_DIR_INO 6
#    define EXT4_RESIZE_INO 7
#    define EXT4_JOURNAL_INO 8
#    define EXT4_GOOD_OLD_FIRST_INO 11

#    define EXT4_LINK_MAX 65000

#    define EXT4_MIN_BLOCK_SIZE 1024
#    define EXT4_MAX_BLOCK_SIZE 65536
#    define EXT4_MIN_BLOCK_LOG_SIZE 10
#    define EXT4_MAX_BLOCK_LOG_SIZE 16
#    ifdef __KERNEL__
#        define EXT4_BLOCK_SIZE(s) ((s)->s_blocksize)
#    else
#        define EXT4_BLOCK_SIZE(s)                                             \
            (EXT4_MIN_BLOCK_SIZE << (s)->s_log_block_size)
#    endif  // __KERNEL__

#    define EXT4_ADDR_PER_BLOCK(s) (EXT4_BLOCK_SIZE(s) / sizeof(__u32))
#    define EXT4_CLUSTER_SIZE(s)                                               \
        (EXT4_BLOK_SIZE(s) << EXT4_SB(s)->s_cluseter_bits)
#    ifdef __KERNEL
#        define EXT4_BLOCK_SIZE_BITS(s) ((s)->s_blocksize_bits)
#        define EXT4_CLUSTER_BITS(s) (EXT4_SB(s)->s_cluster_bits)
#    else
#        define EXT4_BLOCK_SIZE_BITS(s) ((s)->s_log_block_size + 10)
#    endif  // __KERNEL
#    ifdef __KERNEL__
#        define EXT4_ADDR_PER_BLOCK_BITS(s) (EXT4_SB(s)->s_addr_per_block_bits)
#        define EXT4_INODE_SIZE(s) (EXT4_SB(s)->s_inode_size)
#        define EXT4_FIRST_INO(s) (EXT4_SB(s)->s_first_ino)
#    else
#        define EXT4_INODE_SIZE(s)                                             \
            (((s)->s_rev_level == EXT4_GOOD_OLD_REV) ?                         \
                 EXT4_GOOD_OLD_INODE_SIZE :                                    \
                 (s)->s_inode_size)
#        define EXT4_FIRST_INO(s)                                              \
            (((s)->s_rev_level == EXT4_GOOD_OLD_REV) ?                         \
                 EXT4_GOOD_OLD_FIRST_INO :                                     \
                 (s)->s_first_ino)
#    endif  // __KERNEL__
#    define EXT4_BLOCK_ALIGN(size, blkbits) ALIGN((size), (1 << (blkbits)))

#    define EXT4_B2C(sbi, blk) ((blk) >> (sbi)->s_cluster_bits)
#    define EXT4_C2B(sbi, cluster) ((cluster) << (sbi)->s_cluster_bits)
#    define EXT4_NUM_B2C(sbi, blks)                                            \
        (((blks) + (sbi)->s_cluster_ratio - 1) >> (sbi)->s_cluster_bits)

struct ext4_group_desc {
    __le32 bg_block_bitmap_lo;
    __le32 bg_inode_bitmap_lo;
    __le32 bg_block_bitmap_lo;
    __le32 bg_inode_bitmap_lo;
    __le32 bg_inode_table_lo;
    __le16 bg_free_blocks_count_lo;
    __le16 bg_free_inodes_count_lo;
    __le16 bg_used_dirs_count_lo;
    __le16 bg_flags;

    __le32 bg_exclude_bitmap_lo;
    __le16 bg_block_bitmap_csum_lo;

    __le16 bg_inode_bitmap_csum_lo;

    __le16 bg_itable_unused_lo;
    __le16 bg_checksum;
    __le32 bg_block_bitmap_hi;
    __le32 bg_inode_bitmap_hi;
    __le32 bg_inode_table_hi;
    __le16 bg_free_blocks_count_hi;
    __le16 bg_free_inodes_count_hi;
    __le16 bg_used_dirs_count_hi;
    __le16 bg_itable_unused_hi;
    __le32 bg_exclude_bitmap_hi;
    __le16 bg_block_bitmap_csum_hi;

    __le16 bg_inode_bitmap_csum_hi;

    __u32 bg_reserved;
};

#    define EXT4_BG_INODE_BITMAP_CSUM_HI_END                                   \
        (offsetof(struct ext4_group_desc, bg_inode_bitmap_csum_hi)             \
         + sizeof(__le16))
#    define EXT4_BG_BLOCK_BITMAP_CSUM_HI_END                                   \
        (offsetof(struct ext4_group_desc, bg_block_bitmap_csum_hi)             \
         + sizeof(__le16))

struct flex_groups {
    atomic64_t free_clusters;
    atomic_t free_inodes;
    atomic_t used_dirs;
};

#    define EXT4_BG_INODE_UNINIT 0x0001
#    define EXT4_BG_BLOCK_UNINIT 0x0002
#    define EXT4_BG_INODE_ZEROED 0x0004

#    define EXT4_MIN_DESC_SIZE 32
#    define EXT4_MIN_DESC_SIZE_64BIT 64
#    define EXT4_MAX_DESC_SIZE EXT4_MIN_BLOCK_SIZE
#    define EXT4_DESC_SIZE(s) (EXT4_SB(s)->s_desc_size)
#    ifdef __KERNEL__
#        define EXT4_BLOCKS_PER_GROUP(s) (EXT4_SB(s)->s_blocks_per_group)
#        define EXT4_CLUSTERS_PER_GROUP(s) (EXT4_SB(s)->s_cluster_per_group)
#        define EXT4_DESC_PER_BLOCK(s) (EXT4_SB(s)->s_desc_per_block)
#        define EXT4_INODES_PER_GROUP(s) (EXT4_SB(s)->s_inodes_per_group)
#        define EXT4_DESC_PER_BLOCK_BITS(s) (EXT4_SB(s)->s_desc_per_block_bits)
#    else
#        define EXT4_BLOCKS_PER_GROUP(s) ((s)->s_blocks_per_group)
#        define EXT4_DESC_PER_BLOCK(s) (EXT4_BLOCK_SIZE(s) / EXT4_DESC_SIZE(s))
#        define EXT4_INODES_PER_GROUP(s) ((s)->s_inodes_per_group)
#    endif  // __KERNEL__

#    define EXT4_NDIR_BLOCKS 12
#    define EXT4_IND_BLOCK EXT4_NDIR_BLOCKS
#    define EXT4_DIND_BLOCK (EXT4_IND_BLOCK + 1)
#    define EXT4_TIND_BLOCK (EXT4_DIND_BLOCK + 1)
#    define EXT4_N_BLOCKS (EXT4_TIND_BLOCK + 1)

#    define EXT4_SECRM_FL 0x00000001
#    define EXT4_UNRM_FL 0x00000002
#    define EXT4_COMPR_FL 0x00000004
#    define EXT4_SYNC_FL 0x00000008
#    define EXT4_IMMUTABLE_FL 0x00000010
#    define EXT4_APPEND_FL 0x00000020
#    define EXT4_NODUMP_FL 0x00000040
#    define EXT4_NOATIME_FL 0x00000080
#    define EXT4_DIRTY_FL 0x00000100
#    define EXT4_COMPRBLK_FL 0x00000200
#    define EXT4_NOCOMPR_FL 0x00000400
#    define EXT4_ECOMPR_FL 0x00000800
#    define EXT4_INDEX_FL 0x00001000
#    define EXT4_IMAGIC_FL 0x00002000
#    define EXT4_JOURNAL_DATA_FL 0x00004000
#    define EXT4_NOTAIL_FL 0x00008000
#    define EXT4_DIRSYNC_FL 0x00010000
#    define EXT4_TOPDIR_FL 0x00020000
#    define EXT4_HUGE_FILE_FL 0x00040000
#    define EXT4_EXTENTS_FL 0x00080000
#    define EXT4_EA_INODE_FL 0x00200000
#    define EXT4_EOFBLOCKS_FL 0x00400000
#    define EXT4_INLINE_DATA_FL 0x10000000
#    define EXT4_RESERVED_FL 0x80000000

#    define EXT4_FL_USER_VISIBLE 0x004BDFFF
#    define EXT4_FL_USER_MODIFIABLE 0x004380FF

#    define EXT4_FL_INHERITED                                                  \
        (EXT4_SECRM_FL | EXT4_UNRM_FL | EXT4_COMPR_FL | EXT4_SYNC_FL           \
         | EXT4_NODUMP_FL | EXT4_NOATIME_FL | EXT4_NOCOMPR_FL                  \
         | EXT4_JOURNAL_DATA_FL | EXT4_NOTAIL_FL | EXT4_DIRSYNC_FL)

#    define EXT4_REG_FLMASK (~(EXT4_DIRSYNC_FL | EXT4_TOPDIR_FL))

#    define EXT4_OTHER_FLMASK (EXT4_NODUMP_FL | EXT4_NOATIME_FL)

static inline __u32
ext4_mask_flags(umode_t mode, __u32 flags)
{
    if (S_ISDIR(mode))
        return flags;
    else if (S_ISREG(mode))
        return flags & EXT4_REG_FLMASK;
    else
        return flags & EXT4_OTHER_FLMASK;
}

enum {
    EXT4_INODE_SECRM        = 0,
    EXT4_INODE_UNRM         = 1,
    EXT4_INODE_COMPR        = 2,
    EXT4_INODE_SYNC         = 3,
    EXT4_INODE_IMMUTABLE    = 4,
    EXT4_INODE_APPEND       = 5,
    EXT4_INODE_NODUMP       = 6,
    EXT4_INODE_NOATIME      = 7,
    EXT4_INODE_DIRTY        = 8,
    EXT4_INODE_COMPRBLK     = 9,
    EXT4_INODE_UNCOMPR      = 10,
    EXT4_INODE_ECOMPR       = 11,
    EXT4_INODE_INDEX        = 12,
    EXT4_INODE_IMAGIC       = 13,
    EXT4_INODE_JOURNAL_DATA = 14,
    EXT4_INODE_NOTAIL       = 15,
    EXT4_INODE_DIRSYNC      = 16,
    EXT4_INODE_TOPDIR       = 17,
    EXT4_INODE_HUGE_FILE    = 18,
    EXT4_INODE_EXTENTS      = 19,
    EXT4_INODE_EA_INODE     = 21,
    EXT4_INODE_EOFBLOCKS    = 22,
    EXT4_INODE_INLINE_DATA  = 28,
    EXT4_INODE_RESERVED     = 31,
};

#    define TEST_FLAG_VALUE(FLAG) (EXT4_##FLAG##_FL == (1 << EXT4_INODE_##FLAG))
#    define CHECK_FLAG_VALUE(FLAG) BUILD_BUG_ON(!TEST_FLAG_VALUE(FLAG))

static inline void
ext4_check_flag_values(void)
{
    CHECK_FLAG_VALUE(SECRM);
    CHECK_FLAG_VALUE(UNRM);
    CHECK_FLAG_VALUE(COMPR);
    CHECK_FLAG_VALUE(SYNC);
    CHECK_FLAG_VALUE(IMMUTABLE);
    CHECK_FLAG_VALUE(APPEND);
    CHECK_FLAG_VALUE(NODUMP);
    CHECK_FLAG_VALUE(NOATIME);
    CHECK_FLAG_VALUE(DIRTY);
    CHECK_FLAG_VALUE(COMPRBLK);
    CHECK_FLAG_VALUE(NOCOMPR);
    CHECK_FLAG_VALUE(ECOMPR);
    CHECK_FLAG_VALUE(INDEX);
    CHECK_FLAG_VALUE(IMAGIC);
    CHECK_FLAG_VALUE(JOURNAL_DATA);
    CHECK_FLAG_VALUE(NOTAIL);
    CHECK_FLAG_VALUE(DIRSYNC);
    CHECK_FLAG_VALUE(TOPDIR);
    CHECK_FLAG_VALUE(HUGE_FILE);
    CHECK_FLAG_VALUE(EXTENTS);
    CHECK_FLAG_VALUE(EA_INODE);
    CHECK_FLAG_VALUE(EOFBLOCKS);
    CHECK_FLAG_VALUE(INLINE_DATA);
    CHECK_FLAG_VALUE(RESERVED);
}

struct ext4_new_group_input {
    __u32 group;
    __u64 block_bitmap;
    __u64 inode_bitmap;
    __u64 inode_table;
    __u32 blocks_count;
    __u16 reserved_blocks;
    __u16 unused;
};

#    if defined(__KERNEL__) && defined(CONFIG_COMPAT)
struct compat_ext4_new_group_input {
    u32 group;
    compat_u64 block_bitmap;
    compat_u64 inode_bitmap;
    compat_u64 inode_table;
    u32 blocks_count;
    u16 reserved_blocks;
    u16 unused;
};
#    endif

struct ext4_new_group_data {
    __u32 group;
    __u64 block_bitmap;
    __u64 inode_bitmap;
    __u64 inode_table;
    __u32 blocks_count;
    __u16 reserverd_blocks;
    __u16 unused;
    __u32 free_blocks_count;
};

enum {
    BLOCK_BITMAP = 0,
    INODE_BITMAP,
    INODE_TALBE,
    GROUP_TABLE_COUNT,
};

#    define EXT4_GET_BLOCKS_CREATE 0x0001
#    define EXT4_GET_BLOCKS_UNINIT_EXT 0x0002
#    define EXT4_GET_BLOCKS_CREATE_UNINIT_EXT                                  \
        (EXT4_GET_BLOCKS_UNINIT_EXT | EXT4_GET_BLOCKS_CREATE)
#    define EXT4_GET_BLOCKS_DELALLOC_RESERVE 0x0004
#    define EXT4_GET_BLOCKS_PER_IO 0x0008
#    define EXT4_GET_BLOCKS_CONVERT 0x0010
#    define EXT4_GET_BLOCKS_IO_CREATE_EXT                                      \
        (EXT4_GET_BLOCKS_PER_IO | EXT4_GET_BLOCKS_CREATE_UNINIT_EXT)
#    define EXT4_GET_BLOCKS_IO_CONVERT_EXT                                     \
        (EXT4_GET_BLOCKS_CONVERT | EXT4_GET_BLOCKS_CREATE_UNINIT_EXT)
#    define EXt4_GET_BLOCKS_METADATA_NOFAIL 0x0020
#    define EXT4_GET_BLOCKS_NO_NORMALLIZE 0x0040
#    define EXT4_GET_BLOCKS_KEEP_SIZE 0x0080
#    define EXT4_GET_BLOCKS_NO_LOCK 0x0100
#    define EXT4_GET_BLOCKS_NO_PUT_HOLE 0x0200

#    define EXT4_EX_NOCACHE 0x0400
#    define EXT4_EX_FORCE_CACHE 0x0800

#    define EXT4_FREE_BLOCKS_MEATDATA 0x0001
#    define EXT4_FREE_BLOCKS_FORGET 0x0002
#    define EXT4_FREE_BLOCKS_VALIDATED 0x0004
#    define EXT4_FREE_BLOCKS_NO_QUOT_UPDATE 0x0008
#    define EXT4_FREE_BLOCKS_NOFREE_FIRST_CLUSTER 0x0010
#    define EXT4_FREE_BLOCKS_NOFREE_LAST_CLUSTER 0x0020
#    define EXT4_FREE_BLOCKS_RESERVE 0x0040

#    define EXT4_IOC_GETFLAGS FS_IOC_GETFLAGS
#    define EXT4_IOC_SETFLAGS FS_IOC_SETFLAGS
#    define EXT4_IOC_GETVERSION _IOR('f', 3, long)
#    define EXT4_IOC_SETVERSION _IOW('f', 4, long)
#    define EXT4_IOC_GETVERSION_OLD FS_IOC_GETVERSION
#    define EXT4_IOC_SETVERSION_OLD FS_IOC_SETVERSION
#    define EXT4_IOC_GETRSVSZ _IOR('f', 5, long)
#    define EXT4_IOC_SET_RSVSZ _IOW('f', 6, long)
#    define EXT4_IOC_GROUP_EXTEND _IOW('f', 7, unsigned long)
#    define EXT4_IOC_GROUP_ADD _IOW('f', 8, struct ext4_new_group_input)
#    define EXT4_IOC_MIGRATE _IO('f', 9)
#    define EXT4_IOC_ALLOC_DA_BLKS _IO('f', 12)
#    define EXT4_IOC_MOVE_EXT _IOWR('f', 15, struct move_extent)
#    define EXT4_IOC_RESIZE_FS _IOW('f', 16, __u64)
#    define EXT4_IOC_SWAP_BOOT _IO('f', 17)
#    define EXT4_IOC_PRECACHE_EXTENTS _IO('f', 18)

#    if defined(__KERNEL__) && defined(CONFIG_COMPAT)
#        define EXT4_IOC32_GETFLAGS

#        DIRTY  // EXT4__H__
