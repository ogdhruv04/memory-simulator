#ifndef CACHE_H
#define CACHE_H

#include <vector>
#include <list>
#include <unordered_map>
#include <string>

// Cache replacement policy
enum class ReplacementPolicy {
    FIFO,   // First In First Out
    LRU     // Least Recently Used
};

// Single cache line
struct CacheLine {
    bool valid;
    size_t tag;
    size_t last_access;  // For LRU
    
    CacheLine() : valid(false), tag(0), last_access(0) {}
};

// Statistics for a single cache level
struct CacheStats {
    size_t hits;
    size_t misses;
    size_t accesses;
    
    CacheStats() : hits(0), misses(0), accesses(0) {}
    
    double hitRatio() const {
        return accesses > 0 ? (double)hits / accesses * 100.0 : 0.0;
    }
};

// Single cache level (L1, L2, etc.)
class CacheLevel {
private:
    std::string name;
    size_t size;              // Total cache size
    size_t block_size;        // Block/line size
    size_t associativity;     // 1 = direct-mapped, >1 = set-associative
    size_t num_sets;
    size_t num_lines;
    
    std::vector<std::vector<CacheLine>> sets;
    std::vector<std::list<size_t>> fifo_queues;  // For FIFO replacement
    
    ReplacementPolicy policy;
    CacheStats stats;
    size_t access_counter;
    
    // Get set index from address
    size_t getSetIndex(size_t address) const;
    
    // Get tag from address
    size_t getTag(size_t address) const;
    
    // Find a line to evict using current policy
    size_t findVictim(size_t set_index);

public:
    CacheLevel(const std::string& levelName, size_t cacheSize, 
               size_t blockSize, size_t assoc, ReplacementPolicy pol);
    
    // Access cache, returns true on hit, false on miss
    bool access(size_t address);
    
    // Get statistics
    CacheStats getStats() const;
    
    // Reset statistics
    void resetStats();
    
    // Get cache info string
    std::string getInfo() const;
    
    std::string getName() const { return name; }
};

// Multilevel cache simulator
class CacheSimulator {
private:
    std::vector<CacheLevel*> levels;
    bool initialized;

public:
    CacheSimulator();
    ~CacheSimulator();
    
    // Add a cache level
    void addLevel(const std::string& name, size_t size, 
                  size_t blockSize, size_t associativity,
                  ReplacementPolicy policy);
    
    // Access memory address through cache hierarchy
    void access(size_t address);
    
    // Print statistics for all levels
    void printStats() const;
    
    // Print cache configuration
    void printConfig() const;
    
    // Reset all statistics
    void resetStats();
    
    bool isInitialized() const { return initialized; }
};

#endif // CACHE_H
