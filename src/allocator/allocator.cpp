#include "allocator.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

Allocator::Allocator() 
    : head(nullptr), total_size(0), strategy(AllocationStrategy::FIRST_FIT),
      next_block_id(1), stats() {}

Allocator::~Allocator() {
    // Free all memory blocks
    MemoryBlock* current = head;
    while (current != nullptr) {
        MemoryBlock* next = current->next;
        delete current;
        current = next;
    }
}

bool Allocator::initMemory(size_t size) {
    // Clean up existing memory if any
    MemoryBlock* current = head;
    while (current != nullptr) {
        MemoryBlock* next = current->next;
        delete current;
        current = next;
    }
    
    // Create a single free block representing all memory
    head = new MemoryBlock(0, size, true, -1);
    total_size = size;
    next_block_id = 1;
    stats = AllocationStats();
    stats.total_memory = size;
    stats.free_memory = size;
    
    std::cout << "Memory initialized: " << size << " bytes\n";
    return true;
}

void Allocator::setStrategy(AllocationStrategy strat) {
    strategy = strat;
}

void Allocator::setStrategy(const std::string& strategyName) {
    if (strategyName == "first_fit") {
        strategy = AllocationStrategy::FIRST_FIT;
    } else if (strategyName == "best_fit") {
        strategy = AllocationStrategy::BEST_FIT;
    } else if (strategyName == "worst_fit") {
        strategy = AllocationStrategy::WORST_FIT;
    } else {
        std::cout << "Unknown strategy: " << strategyName << "\n";
        std::cout << "Available: first_fit, best_fit, worst_fit\n";
        return;
    }
    std::cout << "Allocator set to: " << getStrategyName() << "\n";
}

std::string Allocator::getStrategyName() const {
    switch (strategy) {
        case AllocationStrategy::FIRST_FIT: return "First Fit";
        case AllocationStrategy::BEST_FIT: return "Best Fit";
        case AllocationStrategy::WORST_FIT: return "Worst Fit";
        default: return "Unknown";
    }
}

MemoryBlock* Allocator::firstFit(size_t size) {
    MemoryBlock* current = head;
    while (current != nullptr) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

MemoryBlock* Allocator::bestFit(size_t size) {
    MemoryBlock* best = nullptr;
    MemoryBlock* current = head;
    
    while (current != nullptr) {
        if (current->is_free && current->size >= size) {
            if (best == nullptr || current->size < best->size) {
                best = current;
            }
        }
        current = current->next;
    }
    return best;
}

MemoryBlock* Allocator::worstFit(size_t size) {
    MemoryBlock* worst = nullptr;
    MemoryBlock* current = head;
    
    while (current != nullptr) {
        if (current->is_free && current->size >= size) {
            if (worst == nullptr || current->size > worst->size) {
                worst = current;
            }
        }
        current = current->next;
    }
    return worst;
}

MemoryBlock* Allocator::findFreeBlock(size_t size) {
    switch (strategy) {
        case AllocationStrategy::FIRST_FIT:
            return firstFit(size);
        case AllocationStrategy::BEST_FIT:
            return bestFit(size);
        case AllocationStrategy::WORST_FIT:
            return worstFit(size);
        default:
            return firstFit(size);
    }
}

int Allocator::allocate(size_t size) {
    if (head == nullptr) {
        std::cout << "Error: Memory not initialized\n";
        return -1;
    }
    
    if (size == 0) {
        std::cout << "Error: Cannot allocate 0 bytes\n";
        return -1;
    }
    
    MemoryBlock* block = findFreeBlock(size);
    
    if (block == nullptr) {
        std::cout << "Allocation failed: No suitable free block for size " << size << "\n";
        stats.allocation_failures++;
        return -1;
    }
    
    int allocated_id = next_block_id++;
    
    // If the block is larger than needed, split it
    if (block->size > size) {
        MemoryBlock* new_free = new MemoryBlock(
            block->address + size,
            block->size - size,
            true,
            -1
        );
        
        new_free->next = block->next;
        new_free->prev = block;
        
        if (block->next != nullptr) {
            block->next->prev = new_free;
        }
        block->next = new_free;
        block->size = size;
    }
    
    // Mark block as allocated
    block->is_free = false;
    block->block_id = allocated_id;
    
    stats.num_allocations++;
    updateStats();
    
    std::cout << "Allocated block id=" << allocated_id 
              << " at address=0x" << std::hex << std::setfill('0') 
              << std::setw(4) << block->address << std::dec 
              << " size=" << size << "\n";
    
    return allocated_id;
}

void Allocator::coalesce(MemoryBlock* block) {
    if (block == nullptr || !block->is_free) return;
    
    // Merge with next block if it's free
    while (block->next != nullptr && block->next->is_free) {
        MemoryBlock* next = block->next;
        block->size += next->size;
        block->next = next->next;
        if (next->next != nullptr) {
            next->next->prev = block;
        }
        delete next;
    }
    
    // Merge with previous block if it's free
    while (block->prev != nullptr && block->prev->is_free) {
        MemoryBlock* prev = block->prev;
        prev->size += block->size;
        prev->next = block->next;
        if (block->next != nullptr) {
            block->next->prev = prev;
        }
        delete block;
        block = prev;
    }
}

bool Allocator::free(int block_id) {
    if (head == nullptr) {
        std::cout << "Error: Memory not initialized\n";
        return false;
    }
    
    // Find the block with given ID
    MemoryBlock* current = head;
    while (current != nullptr) {
        if (current->block_id == block_id && !current->is_free) {
            current->is_free = true;
            current->block_id = -1;
            
            stats.num_deallocations++;
            
            // Coalesce with adjacent free blocks
            coalesce(current);
            
            updateStats();
            std::cout << "Block " << block_id << " freed and merged\n";
            return true;
        }
        current = current->next;
    }
    
    std::cout << "Error: Block " << block_id << " not found\n";
    return false;
}

void Allocator::updateStats() {
    stats.used_memory = 0;
    stats.free_memory = 0;
    
    size_t largest_free = 0;
    size_t total_free = 0;
    int free_block_count = 0;
    
    MemoryBlock* current = head;
    while (current != nullptr) {
        if (current->is_free) {
            stats.free_memory += current->size;
            total_free += current->size;
            free_block_count++;
            if (current->size > largest_free) {
                largest_free = current->size;
            }
        } else {
            stats.used_memory += current->size;
        }
        current = current->next;
    }
    
    // External fragmentation: 1 - (largest_free / total_free)
    if (total_free > 0 && free_block_count > 1) {
        stats.external_fragmentation = (1.0 - (double)largest_free / total_free) * 100.0;
    } else {
        stats.external_fragmentation = 0.0;
    }
}

AllocationStats Allocator::getStats() const {
    return stats;
}

void Allocator::dumpMemory() const {
    if (head == nullptr) {
        std::cout << "Memory not initialized\n";
        return;
    }
    
    std::cout << "\n=== Memory Dump ===\n";
    MemoryBlock* current = head;
    while (current != nullptr) {
        std::cout << "[0x" << std::hex << std::setfill('0') << std::setw(4) 
                  << current->address << " - 0x" 
                  << std::setw(4) << (current->address + current->size - 1) 
                  << std::dec << "] ";
        
        if (current->is_free) {
            std::cout << "FREE";
        } else {
            std::cout << "USED (id=" << current->block_id << ")";
        }
        std::cout << " [" << current->size << " bytes]\n";
        
        current = current->next;
    }
    std::cout << "==================\n\n";
}

bool Allocator::isInitialized() const {
    return head != nullptr;
}
