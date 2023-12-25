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
  int call_num; 
  memcpy(&call_num, f->esp, sizeof(int)); 
  switch (call_num) {
    case SYS_HALT: {
      shutdown_power_off();
      break;
    } 
    case SYS_CREATE: { 
      char *file; 
      unsigned initial_size; 
      memcpy(&file, (char **)f->esp + 1, sizeof(char *)); 
      memcpy(&initial_size, (unsigned *)f->esp + 2, sizeof(unsigned)); 
      if (thread_current()->executable != NULL && strcmp(thread_current()->executable_name, file) == 0) { 
        f->eax = false; 
        return; 
      } 
      bool success = filesys_create(file, initial_size); 
      f->eax = success; 
      break; 
    } 
    case SYS_OPEN: { 
      char *file; 
      memcpy(&file, (char **)f->esp + 1, sizeof(char *)); 
      if (thread_current()->executable != NULL && strcmp(thread_current()->executable_name, file) == 0) { 
        f->eax = -1; 
        return; 
      } 
      struct file *f = filesys_open(file); 
      if (f == NULL) { 
        f->eax = -1; 
      } else { 
        int fd = process_add_file(f); 
        f->eax = fd; 
      } 
      break; 
    }
    case SYS_READ: { 
      int fd; 
      void *buffer; 
      unsigned size; 
      memcpy(&fd, (int *)f->esp + 1, sizeof(int)); 
      memcpy(&buffer, (void **)f->esp + 2, sizeof(void *)); 
      memcpy(&size, (unsigned *)f->esp + 3, sizeof(unsigned)); 
      int bytes_read = process_read_file(fd, buffer, size); 
      f->eax = bytes_read; 
      break; 
    } 
    case SYS_WRITE: { 
      int fd; 
      void *buffer; 
      unsigned size; 
      memcpy(&fd, (int *)f->esp + 1, sizeof(int)); 
      memcpy(&buffer, (void **)f->esp + 2, sizeof(void *)); 
      memcpy(&size, (unsigned *)f->esp + 3, sizeof(unsigned)); 
      int bytes_written = process_write_file(fd, buffer, size); 
      f->eax = bytes_written; 
      break;
    }
    case SYS_EXIT: { 
      int status; 
      memcpy(&status, (int *)f->esp + 1, sizeof(int)); 
      process_exit(status); 
      break; 
    } 
    case SYS_CLOSE: {
      int fd; 
      memcpy(&fd, (int *)f->esp + 1, sizeof(int)); 
      process_close_file(fd); 
      break; 
    }
    default: { 
      printf ("system call!\n");
      thread_exit ();
    }
  }
}
int write (int fd, const void *buffer, unsigned size) { 
  int bytes_written = file_write (file, buffer, size); 
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
