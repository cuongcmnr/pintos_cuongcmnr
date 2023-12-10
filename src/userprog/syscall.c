#include "userprog/syscall.h"
#include "userprog/process.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
static void syscall_handler (struct intr_frame *);
void syscall_init (void) {
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}
void exit(int status) { 
  struct thread *cur = thread_current(); 
  cur->exit_status = status; 
  thread_exit(); 
}
void check_user(const uint8_t *uaddr) { 
  if (uaddr == NULL || !is_user_vaddr(uaddr)) { 
    exit(-1); 
  } 
}
static void syscall_handler (struct intr_frame *f UNUSED) {
  printf ("system call!\n");
  int syscall_number = *(int *)f->esp;
   switch (syscall_number) {
      case SYS_WRITE:        /* Write to a file. */ 
        {
          check_user ((const uint8_t *) f->esp + 16); 
          int fd = *((int *) f->esp + 5); 
          const void *buffer = (const void *) *((uint32_t *) f->esp + 6); 
          unsigned size = *((unsigned *) f->esp + 7); 
          f->eax = write (fd, buffer, size); 
          break;
        }
    }
  thread_exit ();
}
int write (int fd, const void *buffer, unsigned size) { 
  lock_acquire (&filesys_lock); 
  struct file *file = process_get_file (fd); 
  if (!file) { 
    lock_release (&filesys_lock); 
    return -1; 
  } 
  file_deny_write(file); 
  int bytes_written = file_write (file, buffer, size); 
  file_allow_write(file); 
  lock_release (&filesys_lock); 
  return bytes_written; 
}
bool chdir (const char *dir) {
  struct dir *new_dir = dir_open_name (dir);
  if (new_dir == NULL) {
    return false;
  }
  dir_close (thread_current ()->current_dir);
  thread_current ()->current_dir = new_dir;
  return true; 
}
int open (const char *file) { 
  struct file *f = filesys_open (file); 
  if (f == NULL) { 
    return -1; 
  } 
  if (inode_is_dir (file_get_inode (f))) { 
    dir_close ((struct dir *) f); 
    return -1; 
  }
}
bool remove (const char *file) { 
  struct dir *dir = dir_open_root (); 
  bool success = filesys_remove (file);
  dir_close (dir);
  return success; 
}
bool mkdir (const char *dir) {
  return filesys_create (dir, 0); 
}
bool readdir (int fd, char *name) {
  struct file *f = process_get_file (fd); 
  if (!f || !inode_is_dir (file_get_inode (f))) { 
    return false;
  }
  return dir_readdir ((struct dir *) f, name); 
}
bool isdir (int fd) { 
  struct file *f = process_get_file (fd); 
  if (!f) { 
    return false; 
  } 
  return inode_is_dir (file_get_inode (f)); 
} 
int inumber (int fd) { 
  struct file *f = process_get_file (fd); 
  if (!f) { 
    return -1; 
  } 
  return inode_get_inumber (file_get_inode (f));
} 