# Memory Management Simulator - Design Document

## Overview

This document describes the design and implementation of a Memory Management Simulator that models how an operating system manages physical and virtual memory.

## 1. Architecture

### 1.1 Module Structure

```
┌─────────────────────────────────────────────────────────────┐
│                        main.cpp                              │
│                    (CLI Interface)                           │
└─────────────────────────┬───────────────────────────────────┘
                          │
          ┌───────────────┼───────────────┐
          │               │               │
          ▼               ▼               ▼
    ┌──────────┐   ┌──────────┐   ┌─────────────────┐
    │ Allocator│   │  Cache   │   │ Virtual Memory  │
    │  Module  │   │ Simulator│   │    (Optional)   │
    └──────────┘   └──────────┘   └─────────────────┘
```

### 1.2 Data Flow

```
User Command → Parser → Module Handler → Update State → Output
```

## 2. Physical Memory Simulation

### 2.1 Memory Block Structure

Memory is represented as a doubly-linked list of blocks:

```cpp
struct MemoryBlock {
    size_t address;      // Starting address
    size_t size;         // Block size
    bool is_free;        // Allocation status
    int block_id;        // Unique identifier
    MemoryBlock* next;   // Next block
    MemoryBlock* prev;   // Previous block
};
```

**Why doubly-linked list?**
- O(1) block splitting and merging
- Easy traversal in both directions for coalescing
- Dynamic size (no pre-defined block count)

### 2.2 Memory Layout

```
┌────────────────────────────────────────────────────┐
│  Block 1   │  Block 2   │   Block 3    │  Block 4 │
│  (FREE)    │  (USED)    │   (FREE)     │  (USED)  │
│  100 bytes │  200 bytes │   150 bytes  │  50 bytes│
└────────────────────────────────────────────────────┘
Address: 0x0000    0x0064      0x012C        0x01C2
```

## 3. Allocation Strategies

### 3.1 First Fit

**Algorithm:**
1. Start from the beginning of the memory list
2. Find the first free block that fits the request
3. Allocate (split if necessary)

**Time Complexity:** O(n) worst case, O(1) best case
**Pros:** Fast, simple
**Cons:** Causes fragmentation at the beginning

### 3.2 Best Fit

**Algorithm:**
1. Scan all free blocks
2. Find the smallest block that fits
3. Allocate

**Time Complexity:** O(n)
**Pros:** Minimizes wasted space per allocation
**Cons:** Creates many small unusable fragments

### 3.3 Worst Fit

**Algorithm:**
1. Scan all free blocks
2. Find the largest block
3. Allocate

**Time Complexity:** O(n)
**Pros:** Leaves larger remaining fragments
**Cons:** May run out of large blocks quickly

## 4. Memory Coalescing

When freeing memory, adjacent free blocks are merged:

```
Before free(Block 2):
[FREE][USED][FREE][USED]

After free(Block 2):
[    FREE     ][USED]
(All three merged into one)
```

## 5. Fragmentation Metrics

### 5.1 External Fragmentation

```
External Fragmentation = 1 - (Largest Free Block / Total Free Memory)
```

Example:
- Total free: 500 bytes across 5 blocks
- Largest free block: 200 bytes
- External fragmentation: 1 - 200/500 = 60%

### 5.2 Memory Utilization

```
Utilization = Used Memory / Total Memory × 100%
```

## 6. Cache Simulation

### 6.1 Cache Line Structure

```cpp
struct CacheLine {
    bool valid;           // Line contains valid data
    size_t tag;           // Address tag
    size_t last_access;   // For LRU tracking
};
```

### 6.2 Address Decomposition

For a cache with:
- Block size: B bytes
- Number of sets: S

```
Address: | Tag | Set Index | Block Offset |
         |     | log2(S)   |   log2(B)    |
```

### 6.3 Set-Associative Cache

```
Set 0: [Line 0][Line 1][Line 2][Line 3]  (4-way)
Set 1: [Line 0][Line 1][Line 2][Line 3]
...
```

### 6.4 Replacement Policies

**FIFO:**
- Maintain a queue per set
- Evict the oldest entry
- Simple but doesn't consider access patterns

**LRU:**
- Track last access time per line
- Evict least recently used
- Better hit rate but more complex

## 7. Command Interface

| Command | Description |
|---------|-------------|
| `init memory <size>` | Initialize memory |
| `set allocator <strategy>` | Set allocation algorithm |
| `malloc <size>` | Allocate memory |
| `free <id>` | Free memory block |
| `dump memory` | Show memory state |
| `stats` | Show statistics |

## 8. Future Extensions

### 8.1 Buddy Allocation
- Power-of-2 block sizes
- Efficient splitting and coalescing
- Good for kernel allocators

### 8.2 Virtual Memory
- Page tables
- Address translation
- Page replacement (FIFO, LRU, Clock)

## 9. Limitations

1. Simulation only - no actual memory is managed
2. Single-threaded
3. No persistent storage
4. Simplified cache model (no write policies)

## 10. Testing Strategy

1. Unit tests for each allocation strategy
2. Stress tests with random allocations
3. Edge cases (full memory, zero-size, etc.)
4. Fragmentation analysis over time
