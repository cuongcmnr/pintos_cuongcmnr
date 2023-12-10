#ifndef FILESYS_DIRECTORY_H
#define FILESYS_DIRECTORY_H
#include <stdbool.h>
#include <stddef.h>
#include "devices/block.h"
#include "filesys/off_t.h"
/* Maximum length of a file name component.
   This is the traditional UNIX maximum length.
   After directories are implemented, this maximum length may be
   retained, but much longer full path names must be allowed. */
#define NAME_MAX 14
#define DIR_ENTRY_MAX 64
struct dir_entry {
    bool in_use; // Is this entry in use? 
    bool is_dir; // Is this entry a directory? 
    block_sector_t inode_sector; // The sector number of the inode 
    char name[NAME_MAX + 1]; // The name of the file or directory 
};
struct inode;
struct dir { 
    struct inode *inode; // The inode of this directory 
    off_t pos; // The current position in the directory 
    struct dir_entry entries[DIR_ENTRY_MAX]; // Array of directory entries 
};
/* Opening and closing directories. */
bool dir_create (block_sector_t sector, size_t entry_cnt);
struct dir *dir_open (struct inode *);
struct dir *dir_open_root (void);
struct dir *dir_reopen (struct dir *);
void dir_close (struct dir *);
struct inode *dir_get_inode (struct dir *);

/* Reading and writing. */
bool dir_lookup (const struct dir *, const char *name, struct inode **);
bool dir_add (struct dir *, const char *name, block_sector_t);
bool dir_remove (struct dir *, const char *name);
bool dir_readdir (struct dir *, char name[NAME_MAX + 1]);

#endif /* filesys/directory.h */
