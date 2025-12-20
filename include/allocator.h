#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "memory_block.h"
#include <string>

// Allocation strategy enumeration
enum class AllocationStrategy {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
};

// Statistics for memory allocation
struct AllocationStats {
    size_t total_memory;
    size_t used_memory;
    size_t free_memory;
    size_t num_allocations;
    size_t num_deallocations;
    size_t allocation_failures;
    double external_fragmentation;  // Percentage
    
    AllocationStats() 
        : total_memory(0), used_memory(0), free_memory(0),
          num_allocations(0), num_deallocations(0), 
          allocation_failures(0), external_fragmentation(0.0) {}
};

// Memory Allocator class - manages memory allocation/deallocation
class Allocator {
private:
    MemoryBlock* head;           // Head of the block list
    size_t total_size;           // Total memory size
    AllocationStrategy strategy; // Current allocation strategy
    int next_block_id;           // Next block ID to assign
    AllocationStats stats;       // Statistics
    
    // Find a free block using current strategy
    MemoryBlock* findFreeBlock(size_t size);
    
    // Strategy-specific find functions
    MemoryBlock* firstFit(size_t size);
    MemoryBlock* bestFit(size_t size);
    MemoryBlock* worstFit(size_t size);
    
    // Coalesce adjacent free blocks
    void coalesce(MemoryBlock* block);
    
    // Update statistics
    void updateStats();

public:
    Allocator();
    ~Allocator();
    
    // Initialize memory with given size
    bool initMemory(size_t size);
    
    // Set allocation strategy
    void setStrategy(AllocationStrategy strat);
    void setStrategy(const std::string& strategyName);
    
    // Allocate memory, returns block ID or -1 on failure
    int allocate(size_t size);
    
    // Free memory by block ID
    bool free(int block_id);
    
    // Get statistics
    AllocationStats getStats() const;
    
    // Dump memory state (for visualization)
    void dumpMemory() const;
    
    // Get strategy name
    std::string getStrategyName() const;
    
    // Check if memory is initialized
    bool isInitialized() const;
};

#endif // ALLOCATOR_H
