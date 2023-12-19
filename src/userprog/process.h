#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (int status);
void process_activate (void);
int process_add_file(struct file *file); 
int process_read_file(int fd, void *buffer, unsigned size); 
int process_write_file(int fd, const void *buffer, unsigned size); 
void process_close_file(int fd); 
struct file_elem {
  struct file *file;  /* The file. */
  int fd;             /* The file descriptor. */
  struct list_elem elem;  /* List element. */
  bool deny_write;
};
struct file *process_get_file (int fd);
#endif /* userprog/process.h */
