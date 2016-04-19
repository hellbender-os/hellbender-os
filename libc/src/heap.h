#ifndef __LIBC_HEAP_H__
#define __LIBC_HEAP_H__

#include <stddef.h>

// Allocated memory size must be less than this:
#define HEAP_ALLOC_LIMIT 4096

/* Memory allocated to 8 byte boundary.
 * Allocated memory block structure:
 * 
 *         pointer returned by malloc
 *         |
 *         v
 * size(8) data(16+N*8) size(8)
 *
 * Free memory block structure:
 *
 *         prev and next point to this location.
 *         |
 *         v
 * size(8) prev(8) next(8) .. size(8)
 */

void* _heap_alloc(size_t size);
void _heap_free(void *ptr);

#endif
