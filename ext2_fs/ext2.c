#include "ext2.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>

#define BASE_OFFSET 1024 /* location of the superblock in the first group */

struct super_operations s_op;
struct inode_operations i_op;
struct file_operations f_op;

char fs_name[] = "ext2";

/* Implement functions in s_op, i_op, f_op here */
struct super_block* my_get_superblock(struct file_system_type *fs);

struct file_system_type *initialize_ext2(const char *image_path) {
  /* fill super_operations s_op */
  /* fill inode_operations i_op */
  /* fill file_operations f_op */
  /* for example:
      s_op = (struct super_operations){
        .read_inode = your_read_inode_function,
        .statfs = your_statfs_function,
      };
  */
  myfs.name = fs_name;
  myfs.file_descriptor = open(image_path, O_RDONLY);
  myfs.get_superblock = my_get_superblock;
  /* assign get_superblock function
     for example:
        myfs.get_superblock = your_get_superblock;
  */
  return &myfs;
}

struct super_block* my_get_superblock(struct file_system_type *fs)
{
  // vfs superblock struct
  struct super_block* result_sb = malloc(sizeof(struct super_block));
  // original ext2 superblock
  struct ext2_super_block* ext2_sb = malloc(sizeof(struct ext2_super_block));
  // set read/write head to the point where superblock starts
  lseek(fs->file_descriptor, BASE_OFFSET, SEEK_SET);
  // read superblock
  read(fs->file_descriptor, ext2_sb, sizeof(struct ext2_super_block));
  // check if image is in ext2
  if (ext2_sb->s_magic != EXT2_SUPER_MAGIC)
  {
    fprintf(stderr, "image is not in ext2 file system format");
    return NULL;
  }
  // fill vfs superblock's fields
  result_sb->s_inodes_count = ext2_sb->s_inodes_count;
  result_sb->s_blocks_count = ext2_sb->s_blocks_count;
  result_sb->s_free_blocks_count = ext2_sb->s_free_blocks_count;
  result_sb->s_free_inodes_count = ext2_sb->s_free_inodes_count;
  result_sb->s_first_data_block = ext2_sb->s_first_data_block;
// The block size is computed using this 32bit value as the number of bits to shift left the value 1024. This value may only be non-negative. 
  // (formula taken from https://www.nongnu.org/ext2-doc/ext2.html#s_log_block_size)
  result_sb->s_blocksize = 1024 << ext2_sb->s_log_block_size;
  result_sb->s_blocksize_bits = log2(result_sb->s_blocksize);
  result_sb->s_blocks_per_group = ext2_sb->s_blocks_per_group;
  result_sb->s_inodes_per_group = ext2_sb->s_inodes_per_group;
  result_sb->s_minor_rev_level = ext2_sb->s_minor_rev_level;
  result_sb->s_rev_level = ext2_sb->s_rev_level;
  result_sb->s_first_ino = ext2_sb->s_first_ino;
  result_sb->s_inode_size = ext2_sb->s_inode_size;
  result_sb->s_block_group_nr = ext2_sb->s_block_group_nr;
  // TODO: what to fill s_maxbytes field with?
  result_sb->s_type = fs;
  // TODO: CHECK 
  result_sb->s_op = &s_op;
  // s_flags skipped
  result_sb->s_magic = ext2_sb->s_magic;
  // TODO:  get root dentry
  //result_sb->s_root = 
  
  printf("result_sb->s_blocksize: %ld, s_blocksize_bits: %d\n", result_sb->s_blocksize, result_sb->s_blocksize_bits);
  return result_sb;
}
