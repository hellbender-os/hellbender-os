
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

static inline unsigned get_block_tag(block_t* block) {
  return (*(((uint32_t*)block)-1) & BLOCK_SIZE_MASK);
}

static inline unsigned get_data_size(block_t* block) {
  return get_block_tag(block) * 8 - 8;
}

static inline void set_block_tag(block_t* block, uint32_t tag) {
  *(((uint32_t*)block)-1) = tag;
  *((uint32_t*)(block + (tag & BLOCK_SIZE_MASK) - 1)) = tag;
}

static inline void set_data_size(block_t* block, uint32_t size) {
  set_block_tag(block, size / 8 + 1);
}

static inline unsigned is_valid_block(block_t* block, unsigned tag) {
  unsigned tag2 = *((uint32_t*)(block + tag - 1)) & BLOCK_SIZE_MASK;
  return tag == tag2;
}

static inline block_t* preceding_block(block_t *block) {
  unsigned preceding_tag = *((uint32_t*)(block - 1)) & BLOCK_SIZE_MASK;
  if (preceding_tag) return block - preceding_tag;
  else return NULL;
}

static inline block_t* following_block(smallheap_t *heap, block_t *block) {
  unsigned block_tag = get_block_tag(block);
  block_t* following = block + block_tag;
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
