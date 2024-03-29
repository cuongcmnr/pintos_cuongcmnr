#include "userprog/syscall.h"

void
syscall_init (void) 
{
  lock_init (&fslock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static int
get_user (const uint8_t *uaddr)
{
  int result;

  if (is_user_vaddr (uaddr))
    asm ("movl $1f, %0; movzbl %1, %0; 1:"
        : "=&a" (result) : "m" (*uaddr));
  else
    result = -1;

  if (result == -1)
    {
      thread_current ()->exit_stat->code = -1;
      thread_exit ();
    }
    
  return result;
}

static uintptr_t
get_user_word (const uint8_t *uaddr)
{
  get_user (uaddr);
  get_user (uaddr + sizeof (uintptr_t) - 1);
  return *(uintptr_t *)uaddr;
}

static void
check_user_buf_and_kill (const uint8_t *uaddr, unsigned len)
{
  get_user (uaddr);
  get_user (uaddr + len - 1);
}

static void
check_user_str_and_kill (const char *str)
{
  while (get_user ((const uint8_t *)str) != 0)
    str++;
}

static void
sys_halt (void)
{
  shutdown_power_off ();
}

static void
sys_exit (int exit_status)
{
  thread_current ()->exit_stat->code = exit_status;
  thread_exit ();
}

static pid_t
sys_exec (const char *file)
{
  check_user_str_and_kill (file);
  return process_execute (file);
}

int
sys_wait (pid_t pid)
{
  return process_wait (pid);
}

static bool
sys_create (const char *file, unsigned initial_size)
{
  bool ret;

  check_user_str_and_kill (file);

  lock_acquire (&fslock);
  ret = filesys_create (file, initial_size);
  lock_release (&fslock);
  return ret;
}

static bool
sys_remove (const char *file)
{
  bool ret;

  check_user_str_and_kill (file);

  lock_acquire (&fslock);
  ret = filesys_remove (file);
  lock_release (&fslock);
  return ret;
}

static int
sys_open (const char *file)
{
  struct file *f;
  int ret = -1;

  check_user_str_and_kill (file);

  lock_acquire (&fslock);
  f = filesys_open (file);
  if (f != NULL)
    ret = process_allocate_fd (f);
  lock_release (&fslock);
  return ret;
}

static int
sys_filesize (int fd)
{
  struct file *f;
  int ret = -1;
  
  f = process_get_file (fd);
  if (f != NULL)
    {
      lock_acquire (&fslock);
      ret = file_length (f);
      lock_release (&fslock);
    }
  return ret;
}

static int
sys_read (int fd, void *buffer_, unsigned length)
{
  char *buf = (char *)buffer_;
  struct file *f;
  int ret = -1;

  check_user_buf_and_kill ((uint8_t *)buf, length);

  if (fd == STDOUT_FILENO)
    return -1;

  if (fd == STDIN_FILENO)
    {
      while (length--)
        *(buf++) = input_getc ();
      return length;
    }

  f = process_get_file (fd);
  if (f != NULL)
    {
      lock_acquire (&fslock);
      ret = file_read (f, buf, length);
      lock_release (&fslock);
    }
  return ret;
}

static int
sys_write (int fd, const void *buffer, unsigned length)
{
  struct file *f;
  int ret = -1;

  check_user_buf_and_kill (buffer, length);

  if (fd == STDIN_FILENO)
    return -1;

  if (fd == STDOUT_FILENO)
    {
      putbuf (buffer, length);
      return length;
    }

  f = process_get_file (fd);
  if (f != NULL)
    {
      lock_acquire (&fslock);
      ret = file_write (f, buffer, length);
      lock_release (&fslock);
    }
  return ret;
}

static void
sys_seek (int fd, unsigned position)
{
  struct file *f;
  
  f = process_get_file (fd);
  if (f != NULL)
    {
      lock_acquire (&fslock);
      file_seek (f, position);
      lock_release (&fslock);
    }
}

static unsigned
sys_tell (int fd)
{
  struct file *f;
  int ret = -1;
  
  f = process_get_file (fd);
  if (f != NULL)
    {
      lock_acquire (&fslock);
      ret = file_tell (f);
      lock_release (&fslock);
    }
  return ret;
}

static void
sys_close (int fd)
{
  if (fd != STDIN_FILENO && fd != STDOUT_FILENO)
    {
      struct file *f = process_get_file (fd);
      if (f != NULL)
        {
          lock_acquire (&fslock);
          file_close (f);
          lock_release (&fslock);

          process_close_fd (fd);
        }
    }
}

static void
syscall_handler (struct intr_frame *f) 
{
  switch (get_user_word(f->esp)) {
    case SYS_HALT:
      sys_halt ();
      break;
    case SYS_EXIT:
      sys_exit (
        (int) get_user_word (f->esp + 4)); /* exit_status */
      break;
    case SYS_EXEC:
      f->eax = sys_exec (
        (char *) get_user_word (f->esp + 4)); /* file */
      break;
    case SYS_WAIT:
      f->eax = sys_wait (
        (pid_t) get_user_word (f->esp + 4)); /* pid */
      break;
    case SYS_CREATE:
      f->eax = sys_create (
        (char *) get_user_word (f->esp + 4), /* file */
        (unsigned) get_user_word (f->esp + 8)); /* initial_size */
      break;
    case SYS_REMOVE:
      f->eax = sys_remove (
        (char *) get_user_word (f->esp + 4)); /* file */
      break;
    case SYS_OPEN:
      f->eax = sys_open (
        (char *) get_user_word (f->esp + 4)); /* file */
      break;
    case SYS_FILESIZE:
      f->eax = sys_filesize (
        (int) get_user_word (f->esp + 4)); /* fd */
      break;
    case SYS_READ:
      f->eax = sys_read (
        (int) get_user_word (f->esp + 4), /* fd */
        (void *) get_user_word (f->esp + 8), /* buffer */
        (unsigned) get_user_word (f->esp + 12)); /* length */
      break;
    case SYS_WRITE:
      f->eax = sys_write (
        (int) get_user_word (f->esp + 4), /* fd */
        (void *) get_user_word (f->esp + 8), /* buffer */
        (unsigned) get_user_word (f->esp + 12)); /* length */
      break;
    case SYS_SEEK:
      sys_seek (
        (int) get_user_word (f->esp + 4), /* fd */
        (unsigned) get_user_word (f->esp + 8)); /* position */
      break;
    case SYS_TELL:
      f->eax = sys_tell (
        (int) get_user_word (f->esp + 4)); /* fd */
      break;
    case SYS_CLOSE:
      sys_close (
        (int) get_user_word (f->esp + 4)); /* fd */
      break;
    default:
      printf ("system call!\n");
      break;
  }
}
