#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include <string.h>
#include "filesys/filesys.h"
#include "userprog/process.h"
#include "filesys/directory.h" 
#include "filesys/filesys.h" 
#include "filesys/inode.h"
static inline struct dir *dir_open_name(const char *name) { 
  struct inode *inode = filesys_open(name); 
  return inode != NULL ? dir_open(inode) : NULL; 
}
static inline bool inode_is_dir(struct inode *inode) { 
  return inode->data.is_dir; 
}
void syscall_init (void);

#endif /* userprog/syscall.h */
