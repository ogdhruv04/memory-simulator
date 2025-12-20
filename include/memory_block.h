#ifndef MEMORY_BLOCK_H
#define MEMORY_BLOCK_H

#include <cstddef>

// Represents a block of memory (either free or allocated)
struct MemoryBlock {
    size_t address;      // Starting address of the block
    size_t size;         // Size of the block
    bool is_free;        // Whether this block is free
    int block_id;        // Unique ID for allocated blocks (-1 if free)
    
    MemoryBlock* next;   // Next block in the list
    MemoryBlock* prev;   // Previous block in the list
    
    MemoryBlock(size_t addr, size_t sz, bool free = true, int id = -1)
        : address(addr), size(sz), is_free(free), block_id(id),
          next(nullptr), prev(nullptr) {}
};

#endif // MEMORY_BLOCK_H
