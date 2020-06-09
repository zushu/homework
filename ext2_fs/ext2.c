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

// helper
void seek_inode(int inode_number);
struct ext2_dir_entry* find_ext2_dentry_with_name(struct inode * i_node, char* name);
/* Implement functions in s_op, i_op, f_op here */
// s_op functions
struct super_block* my_get_superblock(struct file_system_type *fs);
int my_statfs(struct super_block *sb, struct kstatfs *ksfs);
void my_read_inode(struct inode* i_node);
// i_op functions
struct dentry* my_lookup(struct inode * i_node, struct dentry * d_entry);

struct file_system_type *initialize_ext2(const char *image_path) {
  /* fill super_operations s_op */
  s_op.statfs = my_statfs;
  s_op.read_inode = my_read_inode;
  /* fill inode_operations i_op */
  i_op.lookup = my_lookup;
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
printf("root_inode->i_size: %lld\n", root_inode->i_size);
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
  printf("block size: %ld \n", result_sb->s_blocksize);
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
  result_sb->s_root->d_parent = result_sb->s_root; // root's parent should point to itself
  result_sb->s_root->d_name = malloc(sizeof(char)*root_dentry_ext2->name_len);
  for (int i = 0; i < root_dentry_ext2->name_len; i++)
  {
    result_sb->s_root->d_name[i] = root_dentry_ext2->name[i];
  }
  result_sb->s_root->d_name = "/";
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

int my_statfs(struct super_block *sb, struct kstatfs *ksfs)
{
  //  TODO: INCOMPLETE
  int name_len = strlen(sb->s_type->name);
  ksfs->f_namelen = name_len;
  ksfs->name = malloc(sizeof(char)*name_len);
  for (int i = 0; i < name_len; i++)
  {
    ksfs->name[i] = sb->s_type->name[i];
  }
  ksfs->f_bsize = sb->s_blocksize;
  ksfs->f_blocks = sb->s_blocks_count;
  ksfs->f_bfree = sb->s_free_blocks_count;
  ksfs->f_inodes = sb->s_inodes_count;
  ksfs->f_finodes = sb->s_free_inodes_count;
  ksfs->f_inode_size = sb->s_inode_size;
  ksfs->f_minor_rev_level = sb->s_minor_rev_level;
  ksfs->f_rev_level = sb->s_rev_level;

  printf("ksfs name: %s", ksfs->name);
  ksfs->f_magic = sb->s_magic;

  return 0;
}

void my_read_inode(struct inode* i_node)
{
  seek_inode(i_node->i_ino);
    // READ INODE FROM DISK
  struct ext2_inode* inode_ext2 = malloc(sizeof(struct ext2_inode));
  read(current_fs->file_descriptor, inode_ext2, sizeof(struct ext2_inode));
  i_node->i_mode = inode_ext2->i_mode;
  i_node->i_nlink = inode_ext2->i_links_count;
  i_node->i_uid = inode_ext2->i_uid;
  i_node->i_gid = inode_ext2->i_gid;
  i_node->i_size = inode_ext2->i_size;
  i_node->i_atime = inode_ext2->i_atime;
  i_node->i_mtime = inode_ext2->i_mtime;
  i_node->i_ctime = inode_ext2->i_ctime;
  i_node->i_blocks = inode_ext2->i_blocks;
  unsigned int num_blocks = 	i_node->i_blocks / (current_sb->s_blocksize/512);
  for(unsigned int i = 0; i < num_blocks; i++)
  {
    i_node->i_block[i] = inode_ext2->i_block[i];
  }
  i_node->i_op = &i_op;
  i_node->f_op = &f_op;
  i_node->i_sb = current_sb;
  // i_state skipped
  i_node->i_flags = inode_ext2->i_flags;

  printf("i_node->i_ino: %ld\n", i_node->i_ino);
  printf("i_node->i_blocks: %ld\n", i_node->i_blocks);
  printf("i_node->i_size: %lld\n", i_node->i_size);
  free(inode_ext2);
}

/******************************
 * INODE OPERATIONS FUNCTIONS *
 ******************************/

struct dentry* my_lookup(struct inode * i_node, struct dentry * d_entry)
{
  // TODO: use recursion in a different function, call in my_lookup
  printf("lookup\n");
  char parent_name[] = "..";
  struct ext2_dir_entry* dentry_ext2 = find_ext2_dentry_with_name(i_node, d_entry->d_name);
  if (dentry_ext2)
  {
    printf("found\n");
    printf("dentry_ext2->name: %s\n", dentry_ext2->name);
    // root dentry
    if (i_node->i_ino == EXT2_ROOT_INO)
    {
        printf("in root\n");
        d_entry->d_parent = current_sb->s_root;
        struct inode* inode_of_dentry = malloc(sizeof(struct inode));
        inode_of_dentry->i_ino = dentry_ext2->inode;
        current_sb->s_op->read_inode(inode_of_dentry);

        d_entry->d_inode = inode_of_dentry;
        d_entry->d_flags = inode_of_dentry->i_flags;
        d_entry->d_sb = current_sb;
        return d_entry;
    }
    
    // get inode of dentry_ext2
    struct inode* inode_of_dentry = malloc(sizeof(struct inode));
    inode_of_dentry->i_ino = dentry_ext2->inode;
    current_sb->s_op->read_inode(inode_of_dentry);


    // find parent of dentry_ext2
    struct dentry* parent_dentry = malloc(sizeof(struct dentry));
    struct ext2_dir_entry* parent_dentry_ext2 = find_ext2_dentry_with_name(inode_of_dentry, parent_name);
    // set name field
    parent_dentry->d_name = malloc(sizeof(char)*(parent_dentry_ext2->name_len + 1));
    parent_dentry->d_name = parent_dentry_ext2->name;
    parent_dentry->d_name[parent_dentry_ext2->name_len] = '\0';
    

   
    printf("parent_dentry name: %s\n", parent_dentry->d_name);

    // get inode of parent_dentry_ext2
    struct inode* inode_of_parent_dentry = malloc(sizeof(struct inode));
    inode_of_parent_dentry->i_ino = parent_dentry_ext2->inode;
    current_sb->s_op->read_inode(inode_of_parent_dentry);

    parent_dentry->d_flags = inode_of_parent_dentry->i_flags;
    parent_dentry->d_inode = inode_of_parent_dentry;
    parent_dentry->d_sb = current_sb;

    d_entry->d_parent = parent_dentry;
    d_entry->d_inode = inode_of_dentry;
    d_entry->d_flags = inode_of_dentry->i_flags;
    d_entry->d_sb = current_sb;  
    return my_lookup(inode_of_parent_dentry, d_entry->d_parent);
  }
  //return d_entry;
  printf("will return null\n");
  return NULL;
}

struct ext2_dir_entry* find_ext2_dentry_with_name(struct inode * i_node, char* name)
{
  short flag_is_found = 0;
  // first 12 blocks of inode
  for (int i = 0; i < 12; i++)
  {
    unsigned int size = 0; // to keep track of the bytes read
      unsigned char block[current_sb->s_blocksize];
      // read the i-th block of inode
      lseek(current_fs->file_descriptor, BASE_OFFSET + current_sb->s_blocksize * (i_node->i_block[i] - 1) + size, SEEK_SET); // TODO: CHECK 
      read(current_fs->file_descriptor, block , current_sb->s_blocksize);
      // dentry_ext2 points to the beginning of the block
      struct ext2_dir_entry* dentry_ext2 = (struct ext2_dir_entry*) block;
      // read each d_entry in each block of inode : inode->i_block[i]
      while (size < i_node->i_size)
      {       
        char* dir_entry_name = malloc((dentry_ext2->name_len+1)*sizeof(char));
        memcpy(dir_entry_name, dentry_ext2->name, dentry_ext2->name_len);
        // create null terminated string
        dir_entry_name[dentry_ext2->name_len] = '\0';
        
        // if the name read from disk is the same as the name we are looking for
        if (strcmp(name, dir_entry_name) == 0)
        {
          flag_is_found = 1;
          return dentry_ext2;
          //break;
        }
        dentry_ext2 = (void*) dentry_ext2 + dentry_ext2->rec_len;      // move to the next ext2 entry
        size += dentry_ext2->rec_len;
      }
  }
  return NULL;
}
// helper
void seek_inode(int inode_number)
{
  struct ext2_group_desc* ext2_gd = malloc(sizeof(struct ext2_group_desc));
  // READ GROUP DESCRIPTORS
  lseek(current_fs->file_descriptor, BASE_OFFSET + current_sb->s_blocksize, SEEK_SET);
  read(current_fs->file_descriptor, ext2_gd, sizeof(struct ext2_group_desc));
  // SEEK INODE
  lseek(current_fs->file_descriptor, BASE_OFFSET + current_sb->s_blocksize * (ext2_gd->bg_inode_table - 1) + (inode_number - 1)*sizeof(struct ext2_inode), SEEK_SET);
  free(ext2_gd);
}
