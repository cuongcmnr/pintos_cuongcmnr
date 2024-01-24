#include "devices/block.h"
#include <stdbool.h>

/* Number of disk blocks stored in the buffer cache. */
#define CACHE_SIZE 64

void cache_init (void);
void cache_read (block_sector_t sector, void *buffer, off_t off, unsigned size);
void cache_write (block_sector_t sector, const void *data,
                  off_t off, unsigned size);
void cache_zero (block_sector_t sector);
void cache_flush (void);
void cache_ra_request (block_sector_t sector);
