#include "devices/block.h"
#include <stdbool.h>
#include <string.h>
#include <random.h>
#include <debug.h>
#include "devices/timer.h"
#include "filesys/filesys.h"
#include "threads/synch.h"
#include "threads/thread.h"
/* Number of disk blocks stored in the buffer cache. */
#define CACHE_SIZE 64

/* Number of milliseconds to wait before flushing all cached data to disk. */
#define WRITE_BEHIND_PERIOD 5000

/* Maximum size of the read-ahead queue. If the queue grows larger than this,
   old requests will be discarded and replaced by new ones (the old ones
   wouldn't be of use anyway, since they would be evicted by new requests
   before having a chance to get used). */
#define RA_QUEUE_SIZE CACHE_SIZE

/* Maximum number of accesses that a slot is allowed to record. Raising this
   constant means that a heavily-accessed slot will get more "second chances"
   when being considered for eviction. Hence, this makes the cache more
   efficient. On the other hand, raising the number too high could make
   eviction take a lot of CPU time. */
#define MAX_ACCESS          5
void cache_init (void);
void cache_read (block_sector_t sector, void *buffer, off_t off, unsigned size);
void cache_write (block_sector_t sector, const void *data,
                  off_t off, unsigned size);
void cache_zero (block_sector_t sector);
void cache_flush (void);
void cache_ra_request (block_sector_t sector);
