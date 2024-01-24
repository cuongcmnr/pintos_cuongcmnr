#ifndef USERPROG_GDT_H
#define USERPROG_GDT_H

#include "threads/loader.h"
#include <debug.h>
#include "userprog/tss.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"
/* Segment selectors.
   More selectors are defined by the loader in loader.h. */
#define SEL_UCSEG       0x1B    /* User code selector. */
#define SEL_UDSEG       0x23    /* User data selector. */
#define SEL_TSS         0x28    /* Task-state segment. */
#define SEL_CNT         6       /* Number of segments. */

void gdt_init (void);
/* The Global Descriptor Table (GDT).

   The GDT, an x86-specific structure, defines segments that can
   potentially be used by all processes in a system, subject to
   their permissions.  There is also a per-process Local
   Descriptor Table (LDT) but that is not used by modern
   operating systems.

   Each entry in the GDT, which is known by its byte offset in
   the table, identifies a segment.  For our purposes only three
   types of segments are of interest: code, data, and TSS or
   Task-State Segment descriptors.  The former two types are
   exactly what they sound like.  The TSS is used primarily for
   stack switching on interrupts.

   For more information on the GDT as used here, refer to
   [IA32-v3a] 3.2 "Using Segments" through 3.5 "System Descriptor
   Types". */
static uint64_t gdt[SEL_CNT];

/* GDT helpers. */
static uint64_t make_code_desc (int dpl);
static uint64_t make_data_desc (int dpl);
static uint64_t make_tss_desc (void *laddr);
static uint64_t make_gdtr_operand (uint16_t limit, void *base);
#endif /* userprog/gdt.h */
