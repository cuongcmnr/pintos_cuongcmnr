#ifndef USERPROG_TSS_H
#define USERPROG_TSS_H

#include <stdint.h>
#include <debug.h>
#include <stddef.h>
#include "userprog/gdt.h"
#include "threads/thread.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"

struct tss;
void tss_init (void);
struct tss *tss_get (void);
void tss_update (void);

#endif /* userprog/tss.h */
