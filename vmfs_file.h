#ifndef VMFS_FILE_H
#define VMFS_FILE_H

#include <sys/stat.h>

/* File types (in inode and directory entries) */
#define VMFS_FILE_TYPE_DIR      0x02
#define VMFS_FILE_TYPE_FILE     0x03
#define VMFS_FILE_TYPE_SYMLINK  0x04

/* === VMFS file abstraction === */
struct vmfs_file {
   const vmfs_fs_t *fs;
   vmfs_blk_list_t blk_list;
   vmfs_inode_t inode;

   /* Current position in file */
   off_t pos;

   /* ... */
};

static inline mode_t vmfs_file_type2mode(m_u32_t type) {
   switch (type) {
   case VMFS_FILE_TYPE_DIR:
      return S_IFDIR;
   case VMFS_FILE_TYPE_SYMLINK:
      return S_IFLNK;
   default:
      return S_IFREG;
   }
}

/* Get file size */
static inline m_u64_t vmfs_file_get_size(const vmfs_file_t *f)
{
   return(f->inode.size);
}

/* Create a file structure */
vmfs_file_t *vmfs_file_create_struct(const vmfs_fs_t *fs);

/* Open a file based on a directory entry */
vmfs_file_t *vmfs_file_open_rec(const vmfs_fs_t *fs,const vmfs_dirent_t *rec);

/* Open a file */
vmfs_file_t *vmfs_file_open(const vmfs_fs_t *fs,const char *filename);

/* Close a file */
int vmfs_file_close(vmfs_file_t *f);

/* Set position */
int vmfs_file_seek(vmfs_file_t *f,off_t pos,int whence);

/* Read data from a file */
ssize_t vmfs_file_read(vmfs_file_t *f,u_char *buf,size_t len);

/* Dump a file */
int vmfs_file_dump(vmfs_file_t *f,off_t pos,size_t len,FILE *fd_out);

/* Get file status */
int vmfs_file_fstat(const vmfs_file_t *f,struct stat *buf);

/* Get file file status (follow symlink) */
int vmfs_file_stat(const vmfs_fs_t *fs,const char *path,struct stat *buf);

/* Get file file status (do not follow symlink) */
int vmfs_file_lstat(const vmfs_fs_t *fs,const char *path,struct stat *buf);

#endif
