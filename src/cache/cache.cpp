#include "cache.h"
#include <iostream>
#include <iomanip>
#include <cmath>

// ============ CacheLevel Implementation ============

CacheLevel::CacheLevel(const std::string& levelName, size_t cacheSize, 
                       size_t blockSize, size_t assoc, ReplacementPolicy pol,
                       size_t latency)
    : name(levelName), size(cacheSize), block_size(blockSize), 
      associativity(assoc), policy(pol), access_counter(0), access_latency(latency) {
    
    num_lines = size / block_size;
    num_sets = num_lines / associativity;
    
    // Initialize cache sets
    sets.resize(num_sets);
    fifo_queues.resize(num_sets);
    
    for (size_t i = 0; i < num_sets; i++) {
        sets[i].resize(associativity);
    }
}

size_t CacheLevel::getSetIndex(size_t address) const {
    // Remove block offset bits, then mod by number of sets
    size_t block_bits = (size_t)std::log2(block_size);
    return (address >> block_bits) % num_sets;
}

size_t CacheLevel::getTag(size_t address) const {
    size_t block_bits = (size_t)std::log2(block_size);
    size_t set_bits = (size_t)std::log2(num_sets);
    return address >> (block_bits + set_bits);
}

size_t CacheLevel::findVictim(size_t set_index) {
    auto& set = sets[set_index];
    
    // First, look for an invalid line
    for (size_t i = 0; i < associativity; i++) {
        if (!set[i].valid) {
            return i;
        }
    }
    
    // All lines valid, use replacement policy
    if (policy == ReplacementPolicy::FIFO) {
        // FIFO: evict the first one that came in
        size_t victim = fifo_queues[set_index].front();
        fifo_queues[set_index].pop_front();
        return victim;
    } else {
        // LRU: find line with smallest last_access
        size_t victim = 0;
        size_t min_access = set[0].last_access;
        for (size_t i = 1; i < associativity; i++) {
            if (set[i].last_access < min_access) {
                min_access = set[i].last_access;
                victim = i;
            }
        }
        return victim;
    }
}

bool CacheLevel::access(size_t address, bool isWrite) {
    stats.accesses++;
    stats.total_access_time += access_latency;  // Always pay the access cost
    access_counter++;
    
    size_t set_index = getSetIndex(address);
    size_t tag = getTag(address);
    auto& set = sets[set_index];
    
    // Check for hit
    for (size_t i = 0; i < associativity; i++) {
        if (set[i].valid && set[i].tag == tag) {
            // Hit!
            stats.hits++;
            set[i].last_access = access_counter;  // Update for LRU
            if (isWrite) {
                set[i].dirty = true;  // Mark as modified
            }
            return true;
        }
    }
    
    // Miss - find victim and replace
    stats.misses++;
    size_t victim = findVictim(set_index);
    
    // Check if victim is dirty (needs write-back)
    if (set[victim].valid && set[victim].dirty) {
        stats.write_backs++;
    }
    
    set[victim].valid = true;
    set[victim].tag = tag;
    set[victim].last_access = access_counter;
    set[victim].dirty = isWrite;  // New line is dirty if this is a write
    
    // Update FIFO queue
    if (policy == ReplacementPolicy::FIFO) {
        fifo_queues[set_index].push_back(victim);
    }
    
    return false;
}

CacheStats CacheLevel::getStats() const {
    return stats;
}

void CacheLevel::resetStats() {
    stats = CacheStats();
}

std::string CacheLevel::getInfo() const {
    std::string info = name + ": ";
    info += std::to_string(size) + " bytes, ";
    info += std::to_string(block_size) + "B blocks, ";
    info += std::to_string(associativity) + "-way, ";
    info += (policy == ReplacementPolicy::FIFO ? "FIFO" : "LRU");
    return info;
}

// ============ CacheSimulator Implementation ============

CacheSimulator::CacheSimulator() : initialized(false), total_access_time(0), memory_latency(100) {}

CacheSimulator::~CacheSimulator() {
    for (auto level : levels) {
        delete level;
    }
}

void CacheSimulator::addLevel(const std::string& name, size_t size, 
                               size_t blockSize, size_t associativity,
                               ReplacementPolicy policy, size_t latency) {
    levels.push_back(new CacheLevel(name, size, blockSize, associativity, policy, latency));
    initialized = true;
    std::cout << "Added cache level: " << levels.back()->getInfo() 
              << " (" << latency << " cycle" << (latency > 1 ? "s" : "") << " latency)\n";
}

void CacheSimulator::access(size_t address, bool isWrite) {
    // Access through cache hierarchy with verbose output
    size_t access_time = 0;
    std::string path = "";
    std::string op = isWrite ? "WRITE" : "READ";
    
    for (size_t i = 0; i < levels.size(); i++) {
        auto level = levels[i];
        access_time += level->getLatency();
        
        if (level->access(address, isWrite)) {
            // Hit at this level
            path += level->getName() + " HIT";
            std::cout << "  [" << op << "] → " << path << " (" << access_time << " cycles)\n";
            total_access_time += access_time;
            return;
        } else {
            // Miss at this level, continue to next
            path += level->getName() + " MISS → ";
        }
    }
    
    // Miss at all levels - access main memory
    access_time += memory_latency;
    path += "MEMORY";
    std::cout << "  [" << op << "] → " << path << " (" << access_time << " cycles)\n";
    total_access_time += access_time;
}

void CacheSimulator::printStats() const {
    std::cout << "\n=== Cache Statistics ===\n";
    for (const auto& level : levels) {
        CacheStats stats = level->getStats();
        std::cout << level->getName() << ":\n";
        std::cout << "  Accesses:    " << stats.accesses << "\n";
        std::cout << "  Hits:        " << stats.hits << "\n";
        std::cout << "  Misses:      " << stats.misses << "\n";
        std::cout << "  Write-backs: " << stats.write_backs << "\n";
        std::cout << "  Hit Rate:    " << std::fixed << std::setprecision(2) 
                  << stats.hitRatio() << "%\n";
        std::cout << "  Access Time: " << stats.total_access_time << " cycles\n";
    }
    std::cout << "------------------------\n";
    std::cout << "Total Access Time: " << total_access_time << " cycles\n";
    std::cout << "Memory Latency:    " << memory_latency << " cycles\n";
    std::cout << "========================\n\n";
}

void CacheSimulator::printConfig() const {
    std::cout << "\n=== Cache Configuration ===\n";
    for (const auto& level : levels) {
        std::cout << "  " << level->getInfo() << "\n";
    }
    std::cout << "===========================\n\n";
}

void CacheSimulator::resetStats() {
    for (auto level : levels) {
        level->resetStats();
    }
    std::cout << "Cache statistics reset\n";
}
