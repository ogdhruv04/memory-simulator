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
    bool dirty;          // Modified bit - true if written to
    size_t tag;
    size_t last_access;  // For LRU
    
    CacheLine() : valid(false), dirty(false), tag(0), last_access(0) {}
};

// Statistics for a single cache level
struct CacheStats {
    size_t hits;
    size_t misses;
    size_t accesses;
    size_t write_backs;      // Dirty lines written back to memory on eviction
    size_t total_access_time;  // Total cycles spent accessing this level
    
    CacheStats() : hits(0), misses(0), accesses(0), write_backs(0), total_access_time(0) {}
    
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
    size_t access_latency;  // Cycles to access this cache level
    
    // Get set index from address
    size_t getSetIndex(size_t address) const;
    
    // Get tag from address
    size_t getTag(size_t address) const;
    
    // Find a line to evict using current policy
    size_t findVictim(size_t set_index);

public:
    CacheLevel(const std::string& levelName, size_t cacheSize, 
               size_t blockSize, size_t assoc, ReplacementPolicy pol,
               size_t latency = 1);
    
    // Access cache, returns true on hit, false on miss
    // isWrite: if true, marks the line as dirty (write-back policy)
    bool access(size_t address, bool isWrite = false);
    
    // Get access latency for this level
    size_t getLatency() const { return access_latency; }
    
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
    size_t total_access_time;    // Cumulative access time across all accesses
    size_t memory_latency;       // Latency for main memory access

public:
    CacheSimulator();
    ~CacheSimulator();
    
    // Add a cache level with configurable latency
    void addLevel(const std::string& name, size_t size, 
                  size_t blockSize, size_t associativity,
                  ReplacementPolicy policy, size_t latency = 1);
    
    // Access memory address through cache hierarchy (verbose output)
    // isWrite: if true, marks the line as dirty (write-back policy)
    void access(size_t address, bool isWrite = false);
    
    // Print statistics for all levels
    void printStats() const;
    
    // Print cache configuration
    void printConfig() const;
    
    // Reset all statistics
    void resetStats();
    
    bool isInitialized() const { return initialized; }
};

#endif // CACHE_H
