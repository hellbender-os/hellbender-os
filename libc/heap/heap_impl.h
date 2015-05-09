
typedef union __attribute__((packed)) tinyblock {
  struct __attribute__((packed)) {
    uint8_t tag;
    uint8_t data[7];
  } used;
  struct __attribute__((packed)) {
    uint8_t tag;
    union tinyblock* next;
    uint8_t filler[3];
  } free;
} tinyblock_t;

typedef struct block {
  struct block* prev;
  struct block* next;
} __attribute__((packed)) block_t;


static inline unsigned is_block_used(block_t* block) {
  return (*(((uint32_t*)block)-1) & BLOCK_RESERVED) != 0;
}

static inline void mark_block_used(block_t* block) {
  *(((uint32_t*)block)-1) |= BLOCK_RESERVED;
}

static inline void mark_block_free(block_t* block) {
  *(((uint32_t*)block)-1) &= ~BLOCK_RESERVED;
}

static inline unsigned get_block_size(block_t* block) {
  return *(((uint32_t*)block)-1) & BLOCK_SIZE_MASK;
}

static inline void set_block_size(block_t* block, uint32_t size) {
  *(((uint32_t*)block)-1) = size;
  *((uint32_t*)(block + (size & BLOCK_SIZE_MASK) - 1)) = size;
}

static inline unsigned is_valid_block(block_t* block, unsigned size) {
  unsigned size2 = *((uint32_t*)(block + size - 1)) & BLOCK_SIZE_MASK;
  return size == size2;
}

static inline block_t* preceding_block(block_t *block) {
  unsigned preceding_size = *((uint32_t*)(block - 1)) & BLOCK_SIZE_MASK;
  if (preceding_size) return block - preceding_size;
  else return NULL;
}

static inline block_t* following_block(smallheap_t *heap, block_t *block) {
  unsigned block_size = get_block_size(block);
  block_t* following = block + block_size;
  if (following != heap->end_block) {
    return following;
  } else return NULL;
}

static inline void add_to_free(smallheap_t *heap, block_t* block, unsigned idx) {
  mark_block_free(block);
  if (idx > 129) idx = 130; // last free list contains all huge blocks.
  block->prev = NULL;
  block->next = heap->free[idx];
  if (block->next) block->next->prev = block;
  heap->free[idx] = block;
}

static inline void remove_from_free(smallheap_t* heap, block_t* block, unsigned idx) {
  if (idx > 129) idx = 130;
  if (block->prev) {
    block->prev->next = block->next;
  } else {
    heap->free[idx] = block->next;
  }
  if (block->next) {
    block->next->prev = block->prev;
  }
}

static inline void push_last_block_into_free(smallheap_t *heap) {
  if (heap->last_idx) {
    block_t* to_add = heap->last_block;
    unsigned add_size = heap->last_idx;
    add_to_free(heap, to_add, add_size);
    heap->last_idx = 0;
  }
}
