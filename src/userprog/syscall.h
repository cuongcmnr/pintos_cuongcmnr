#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "userprog/process.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "lib/kernel/console.h"
#include "lib/user/syscall.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"
/* Very coarse lock to synchronize any access to filesystem code. */
struct lock fslock;

void syscall_init (void);
static void syscall_handler (struct intr_frame *);

static int get_user (const uint8_t *);
static uintptr_t get_user_word (const uint8_t *);
static void check_user_buf_and_kill (const uint8_t *, unsigned);
static void check_user_str_and_kill (const char *);
static void sys_halt (void);
static void sys_exit (int);
static pid_t sys_exec (const char *);
static int sys_wait (pid_t);
static bool sys_create (const char *, unsigned);
static bool sys_remove (const char *);
static int sys_open (const char *);
static int sys_filesize (int);
static int sys_read (int, void *, unsigned);
static int sys_write (int, const void *, unsigned);
static void sys_seek (int, unsigned);
static unsigned sys_tell (int);
static void sys_close (int);
#endif /* userprog/syscall.h */
