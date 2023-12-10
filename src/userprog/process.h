#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);
struct file_elem {
  struct file *file;  /* The file. */
  int fd;             /* The file descriptor. */
  struct list_elem elem;  /* List element. */
  bool deny_write;
};
struct file *process_get_file (int fd);
#endif /* userprog/process.h */
