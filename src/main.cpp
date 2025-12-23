/*
 * Memory Management Simulator
 * 
 * A comprehensive simulator for OS memory management concepts including:
 * - Dynamic memory allocation (First Fit, Best Fit, Worst Fit)
 * - Multilevel cache simulation (L1, L2)
 * - Statistics and fragmentation metrics
 * 
 * Usage: ./memsim
 * Type 'help' for available commands
 */

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

#include "allocator.h"
#include "cache.h"
using namespace std;

// Helper function to split string by spaces
std::vector<std::string> splitCommand(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void printHelp() {
    std::cout << R"(
=== Memory Management Simulator - Help ===

MEMORY COMMANDS:
  init memory <size>         Initialize physical memory (in bytes)
  set allocator <strategy>   Set allocation strategy:
                              - first_fit
                              - best_fit  
                              - worst_fit
  malloc <size>              Allocate memory block of given size
  free <id>                  Free memory block by its ID
  dump memory                Display current memory state
  stats                      Show memory statistics

CACHE COMMANDS:
  init cache                 Initialize default cache hierarchy (L1 + L2)
  cache access <address>     Access memory address through cache
  cache stats                Show cache hit/miss statistics
  cache config               Show cache configuration
  cache reset                Reset cache statistics

GENERAL:
  help                       Show this help message
  clear                      Clear screen
  exit                       Exit the simulator

EXAMPLES:
  > init memory 1024
  > set allocator first_fit
  > malloc 100
  > malloc 200
  > free 1
  > dump memory
  > stats

==========================================
)";
}

void printBanner() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════╗
║         MEMORY MANAGEMENT SIMULATOR                      ║
║         OS Memory Concepts Demonstration                 ║
╚══════════════════════════════════════════════════════════╝
Type 'help' for available commands.

)";
}

int main() {
    Allocator allocator;
    CacheSimulator cacheSimulator;
    
    printBanner();
    
    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break;
        }
        
        std::vector<std::string> tokens = splitCommand(line);
        if (tokens.empty()) continue;
        
        std::string cmd = tokens[0];
        
        // ===== EXIT =====
        if (cmd == "exit" || cmd == "quit") {
            std::cout << "Goodbye!\n";
            break;
        }
        
        // ===== HELP =====
        else if (cmd == "help") {
            printHelp();
        }
        
        // ===== CLEAR =====
        else if (cmd == "clear") {
            std::cout << "\033[2J\033[1;1H";  // ANSI clear screen
        }
        
        // ===== INIT MEMORY =====
        else if (cmd == "init" && tokens.size() >= 3 && tokens[1] == "memory") {
            try {
                size_t size = std::stoull(tokens[2]);
                allocator.initMemory(size);
            } catch (...) {
                std::cout << "Error: Invalid size\n";
            }
        }
        
        // ===== INIT CACHE =====
        else if (cmd == "init" && tokens.size() >= 2 && tokens[1] == "cache") {
            // Initialize default cache hierarchy
            // L1: 256 bytes, 16-byte blocks, 4-way set associative, LRU
            cacheSimulator.addLevel("L1", 256, 16, 4, ReplacementPolicy::LRU);
            // L2: 1024 bytes, 32-byte blocks, 8-way set associative, FIFO
            cacheSimulator.addLevel("L2", 1024, 32, 8, ReplacementPolicy::FIFO);
            std::cout << "Cache hierarchy initialized\n";
        }
        
        // ===== SET ALLOCATOR =====
        else if (cmd == "set" && tokens.size() >= 3 && tokens[1] == "allocator") {
            allocator.setStrategy(tokens[2]);
        }
        
        // ===== MALLOC =====
        else if (cmd == "malloc" && tokens.size() >= 2) {
            if (!allocator.isInitialized()) {
                std::cout << "Error: Memory not initialized. Use 'init memory <size>' first.\n";
            } else {
                try {
                    size_t size = std::stoull(tokens[1]);
                    allocator.allocate(size);
                } catch (...) {
                    std::cout << "Error: Invalid size\n";
                }
            }
        }
        
        // ===== FREE =====
        else if (cmd == "free" && tokens.size() >= 2) {
            try {
                int id = std::stoi(tokens[1]);
                allocator.free(id);
            } catch (...) {
                std::cout << "Error: Invalid block ID\n";
            }
        }
        
        // ===== DUMP MEMORY =====
        else if (cmd == "dump" && tokens.size() >= 2 && tokens[1] == "memory") {
            allocator.dumpMemory();
        }
        
        // ===== STATS =====
        else if (cmd == "stats") {
            if (!allocator.isInitialized()) {
                std::cout << "Memory not initialized\n";
            } else {
                AllocationStats stats = allocator.getStats();
                std::cout << "\n=== Memory Statistics ===\n";
                std::cout << "Allocator:              " << allocator.getStrategyName() << "\n";
                std::cout << "Total memory:           " << stats.total_memory << " bytes\n";
                std::cout << "Used memory:            " << stats.used_memory << " bytes\n";
                std::cout << "Free memory:            " << stats.free_memory << " bytes\n";
                std::cout << "Memory utilization:     " << std::fixed << std::setprecision(1)
                          << (stats.total_memory > 0 ? (double)stats.used_memory / stats.total_memory * 100 : 0)
                          << "%\n";
                std::cout << "Allocations:            " << stats.num_allocations << "\n";
                std::cout << "Deallocations:          " << stats.num_deallocations << "\n";
                std::cout << "Allocation failures:    " << stats.allocation_failures << "\n";
                std::cout << "External fragmentation: " << std::fixed << std::setprecision(1)
                          << stats.external_fragmentation << "%\n";
                std::cout << "=========================\n\n";
            }
        }
        
        // ===== CACHE ACCESS =====
        else if (cmd == "cache" && tokens.size() >= 3 && tokens[1] == "access") {
            if (!cacheSimulator.isInitialized()) {
                std::cout << "Error: Cache not initialized. Use 'init cache' first.\n";
            } else {
                try {
                    size_t address = std::stoull(tokens[2], nullptr, 0);  // Supports hex
                    cacheSimulator.access(address);
                    std::cout << "Accessed address: 0x" << std::hex << address << std::dec << "\n";
                } catch (...) {
                    std::cout << "Error: Invalid address\n";
                }
            }
        }
        
        // ===== CACHE STATS =====
        else if (cmd == "cache" && tokens.size() >= 2 && tokens[1] == "stats") {
            if (!cacheSimulator.isInitialized()) {
                std::cout << "Error: Cache not initialized\n";
            } else {
                cacheSimulator.printStats();
            }
        }
        
        // ===== CACHE CONFIG =====
        else if (cmd == "cache" && tokens.size() >= 2 && tokens[1] == "config") {
            if (!cacheSimulator.isInitialized()) {
                std::cout << "Error: Cache not initialized\n";
            } else {
                cacheSimulator.printConfig();
            }
        }
        
        // ===== CACHE RESET =====
        else if (cmd == "cache" && tokens.size() >= 2 && tokens[1] == "reset") {
            cacheSimulator.resetStats();
        }
        
        // ===== UNKNOWN COMMAND =====
        else {
            std::cout << "Unknown command: " << line << "\n";
            std::cout << "Type 'help' for available commands.\n";
        }
    }
    
    return 0;
}
