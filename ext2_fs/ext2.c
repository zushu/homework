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
  // vfs superblock
  struct super_block* result_sb = malloc(sizeof(struct super_block));
  // original ext2 superblock
  struct ext2_super_block* ext2_sb = malloc(sizeof(struct ext2_super_block));
  // set read/write head to the point where superblock starts
  lseek(fs->file_descriptor, BASE_OFFSET, SEEK_SET);
  // READ SUPERBLOCK
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

  struct ext2_group_desc* ext2_gd = malloc(sizeof(struct ext2_group_desc));
  // READ GROUP DESCRIPTORS
  lseek(fs->file_descriptor, BASE_OFFSET + result_sb->s_blocksize, SEEK_SET);
  read(fs->file_descriptor, ext2_gd, sizeof(struct ext2_group_desc));
  // READ INODE TABLE -> go to inode of root (2nd inode in the table)
  int root_inode_number = 2;
  lseek(fs->file_descriptor, BASE_OFFSET + result_sb->s_blocksize * (ext2_gd->bg_inode_table - 1) + (root_inode_number - 1)*sizeof(struct ext2_inode), SEEK_SET);

  // READ ROOT INODE
  struct ext2_inode* root_inode_ext2 = malloc(sizeof(struct ext2_inode));
  struct inode* root_inode = malloc(sizeof(struct inode));
  read(fs->file_descriptor, root_inode_ext2, sizeof(struct ext2_inode));
  root_inode->i_ino = root_inode_number; // TODO: CHECK
  root_inode->i_mode = root_inode_ext2->i_mode;
  root_inode->i_nlink = root_inode_ext2->i_links_count;
  root_inode->i_uid = root_inode_ext2->i_uid;
  root_inode->i_gid = root_inode_ext2->i_gid;
  root_inode->i_size = root_inode_ext2->i_size;
  root_inode->i_atime = root_inode_ext2->i_atime;
  root_inode->i_mtime = root_inode_ext2->i_mtime;
  root_inode->i_ctime = root_inode_ext2->i_ctime;
  root_inode->i_blocks = root_inode_ext2->i_blocks;
  int num_blocks = 	root_inode->i_blocks / ((1024<<ext2_sb->s_log_block_size)/512);
  for(int i = 0; i < num_blocks; i++)
  {
    root_inode->i_block[i] = root_inode_ext2->i_block[i];
  }
  root_inode->i_op = &i_op;
  root_inode->f_op = &f_op;
  root_inode->i_sb = result_sb;
  // i_state skipped
  root_inode->i_flags = root_inode_ext2->i_flags;

  // TODO:  get root dentry
  printf("block size: %d \n", result_sb->s_blocksize);
  unsigned char* block = malloc(result_sb->s_blocksize);

  struct ext2_dir_entry* root_dentry_ext2;
  /*if (! (root_dentry_ext2 = malloc(sizeof(struct ext2_dir_entry))))
{
    fprintf(stderr, "malloc fail");
    return NULL;
}*/

  // read the first block of root inode
  lseek(fs->file_descriptor, BASE_OFFSET + result_sb->s_blocksize * (root_inode_ext2->i_block[0] - 1), SEEK_SET); // TODO: CHECK 
//  read(fs->file_descriptor, &root_dentry_ext2, result_sb->s_blocksize);
  read(fs->file_descriptor, block , result_sb->s_blocksize);
  root_dentry_ext2 = (struct ext2_dir_entry*) block;

  result_sb->s_root = malloc(sizeof(struct dentry));
  result_sb->s_root->d_flags = root_inode->i_flags; // TODO CHECK
  result_sb->s_root->d_inode = root_inode;
  result_sb->s_root->d_parent = NULL; // root has no parent
  result_sb->s_root->d_name = malloc(sizeof(char)*root_dentry_ext2->name_len);
  for (int i = 0; i < root_dentry_ext2->name_len; i++)
  {
    result_sb->s_root->d_name[i] = root_dentry_ext2->name[i];
  }
  result_sb->s_root->d_sb = result_sb;


  printf("result_sb->s_blocksize: %ld, s_blocksize_bits: %d\n", result_sb->s_blocksize, result_sb->s_blocksize_bits);
  printf("result_sb->s_root->d_name: %s\n", result_sb->s_root->d_name);
  free(ext2_sb);
  free(ext2_gd);
  free(root_inode_ext2);
//  free(root_dentry_ext2);
  free(block);
  return result_sb;

}

/*int my_statfs(struct super_block *sb, struct kstatfs *ksfs)
{
  //  TODO: INCOMPLETE
  ksfs->name = sb->s_type->name;
  ksfs->f_magic = sb->s_magic;
  return 0;
}*/
