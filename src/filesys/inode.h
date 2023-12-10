#ifndef FILESYS_INODE_H
#define FILESYS_INODE_H
/* Identifies an inode. */
#define INODE_MAGIC 0x494e4f44
#define INODE_DIRECT_BLOCKS 16
#include <stdbool.h>
#include "filesys/off_t.h"
#include "devices/block.h"
#include "userprog/process.h"
struct bitmap;
/* On-disk inode.
   Must be exactly BLOCK_SECTOR_SIZE bytes long. */
struct inode_disk{
    block_sector_t start;               /* First data sector. */
    off_t length;                       /* File size in bytes. */
    block_sector_t direct[INODE_DIRECT_BLOCKS];  // Direct pointers to data blocks 
    block_sector_t indirect;  // Pointer to a block of pointers to data blocks 
    block_sector_t doubly_indirect;  // Pointer to a block of pointers to indirect blocks
    bool is_dir;  // Is this inode a directory? 
    unsigned magic;                     /* Magic number. */
    uint32_t unused[125];               /* Not used. */
  };
/* In-memory inode. */
struct inode 
  {
    struct list_elem elem;              /* Element in inode list. */
    block_sector_t sector;              /* Sector number of disk location. */
    int open_cnt;                       /* Number of openers. */
    bool removed;                       /* True if deleted, false otherwise. */
    int deny_write_cnt;                 /* 0: writes ok, >0: deny writes. */
    struct inode_disk data;             /* Inode content. */
    bool is_dir;            /* True if directory, false otherwise. */ 
  };
#endif /* filesys/inode.h */
