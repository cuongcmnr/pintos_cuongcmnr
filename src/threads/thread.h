#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#include <debug.h>
#include <list.h>
#include <stdint.h>
#include "threads/synch.h"
#include "filesys/file.h"
#include "threads/fixed-point.h"
#ifdef FILESYS
#include "devices/block.h"
#endif
/* States in a thread's life cycle. */
enum thread_status
  {
    THREAD_RUNNING,     /* Running thread. */
    THREAD_READY,       /* Not running but ready to run. */
    THREAD_BLOCKED,     /* Waiting for an event to trigger. */
    THREAD_DYING        /* About to be destroyed. */
  };

/* Thread identifier type.
   You can redefine this to whatever type you like. */
typedef int tid_t;
#define TID_ERROR ((tid_t) -1)          /* Error value for tid_t. */

/* Thread priorities. */
#define PRI_MIN 0                       /* Lowest priority. */
#define PRI_DEFAULT 31                  /* Default priority. */
#define PRI_MAX 63                      /* Highest priority. */

/* Thread nice values. */
#define NICE_MIN -20                    /* Lowest nice value. */
#define NICE_DEFAULT 0                  /* Default nice value. */
#define NICE_MAX 20                     /* Highest nice value. */

#define MAX_OPEN_FILES 128

/* A kernel thread or user process.

   Each thread structure is stored in its own 4 kB page.  The
   thread structure itself sits at the very bottom of the page
   (at offset 0).  The rest of the page is reserved for the
   thread's kernel stack, which grows downward from the top of
   the page (at offset 4 kB).  Here's an illustration:

        4 kB +---------------------------------+
             |          kernel stack           |
             |                |                |
             |                |                |
             |                V                |
             |         grows downward          |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             +---------------------------------+
             |              magic              |
             |                :                |
             |                :                |
             |               name              |
             |              status             |
        0 kB +---------------------------------+

   The upshot of this is twofold:

      1. First, `struct thread' must not be allowed to grow too
         big.  If it does, then there will not be enough room for
         the kernel stack.  Our base `struct thread' is only a
         few bytes in size.  It probably should stay well under 1
         kB.

      2. Second, kernel stacks must not be allowed to grow too
         large.  If a stack overflows, it will corrupt the thread
         state.  Thus, kernel functions should not allocate large
         structures or arrays as non-static local variables.  Use
         dynamic allocation with malloc() or palloc_get_page()
         instead.

   The first symptom of either of these problems will probably be
   an assertion failure in thread_current(), which checks that
   the `magic' member of the running thread's `struct thread' is
   set to THREAD_MAGIC.  Stack overflow will normally change this
   value, triggering the assertion. */
/* The `elem' member has a dual purpose.  It can be an element in
   the run queue (thread.c), or it can be an element in a
   semaphore wait list (synch.c).  It can be used these two ways
   only because they are mutually exclusive: only a thread in the
   ready state is on the run queue, whereas only a thread in the
   blocked state is on a semaphore wait list. */
struct thread
  {
    /* Owned by thread.c. */
    tid_t tid;                          /* Thread identifier. */
    enum thread_status status;          /* Thread state. */
    char name[16];                      /* Name (for debugging purposes). */
    uint8_t *stack;                     /* Saved stack pointer. */
    int priority;                       /* Priority. */
    struct list_elem sleep_elem;        /* List element for sleep list. */
    struct list_elem allelem;           /* List element for all threads list. */

    /* Shared between thread.c and synch.c. */
    struct list_elem elem;              /* List element. */

#ifdef USERPROG
    /* Owned by userprog/process.c. */
    uint32_t *pagedir;                  /* Page directory. */

    struct thread *parent;              /* Parent thread. */

    struct lock l;                      /* Lock to protect child_exit_stats. */
    struct list child_exit_stats;       /* List of child exit statuses. */
    struct exit_stat *exit_stat;        /* Exit status. */

    struct file **open_files;           /* File descriptor table. */
    int next_fd;                        /* Next file descriptor to use for opening a file. */

    struct semaphore loaded;            /* Semaphore for signaling process load from executable. */
    struct semaphore exited;            /* Semaphore for signaling process exit. */

    struct file *exec_file;             /* Process's opened executable file. */
#endif
#ifdef FILESYS
  block_sector_t cwd;           /* Current working directory */

  struct list dir_list;  /* List of directories that compose the current working directory */
#endif
    /* For thread_sleep. */
    int64_t ticks;                      /* Tick count for sleeping threads. */

    /* For priority donation. */
    int original_priority;              /* Priority before any donation. */
    struct list locks;                  /* List of locks held by this thread that have
                                           resulted in priority donation to it. */
    struct thread *donated_to;          /* Thread receiving donation from this thread. */
    struct thread *donor;               /* Last thread to donate it's priority to this thread. */

    /* For MLFQ scheduling. */
    int32_t recent_cpu;                 /* Amount of CPU time received "recently". */
    int nice;                           /* Nice value. */

    /* Owned by thread.c. */
    unsigned magic;                     /* Detects stack overflow. */
  };

/* A thread's exit status

   Inserted into parent thread's child exit status list.
   Removed from it after the parent finishes waiting on it,
   or when the parent dies. */
struct exit_stat
  {
    int code;
    tid_t tid;
    struct thread *thread;
    struct list_elem elem;
  };

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
extern bool thread_mlfqs;

void thread_init (void);
void thread_start (void);

void thread_tick (void);
void thread_print_stats (void);

typedef void thread_func (void *aux);
tid_t thread_create (const char *name, int priority, thread_func *, void *);

void thread_block (void);
void thread_unblock (struct thread *);

void thread_sleep (int64_t ticks);

struct thread *thread_current (void);
tid_t thread_tid (void);
const char *thread_name (void);

void thread_exit (void) NO_RETURN;
void thread_yield (void);
void thread_check_priority_and_yield (void);

/* Performs some operation on thread t, given auxiliary data AUX. */
typedef void thread_action_func (struct thread *t, void *aux);
void thread_foreach (thread_action_func *, void *);

int thread_get_priority (void);
void thread_set_priority (int);
void thread_donate_priority (struct thread *, struct lock *);
void thread_revoke_priority (struct lock *);

bool thread_more_func (const struct list_elem *, const struct list_elem *,
                       void *);

int thread_get_nice (void);
void thread_set_nice (int);
int thread_get_recent_cpu (void);
int thread_get_load_avg (void);

#endif /* threads/thread.h */
