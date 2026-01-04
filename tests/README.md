# Test Workloads

This directory contains test workloads for the Memory Management Simulator.

## Running Tests

```bash
# Run a single workload
./build/memsim < tests/workload1_basic.txt

# Save output to file
./build/memsim < tests/workload1_basic.txt > output.txt

# Run all workloads
for f in tests/workload*.txt; do echo "=== $f ==="; ./build/memsim < "$f"; done
```

## Workload Descriptions

### workload1_basic.txt
**Purpose:** Basic memory allocation and deallocation

**Tests:**
- Sequential allocations
- Creating holes via free
- First Fit hole filling
- Block coalescing
- Allocation failure
- Full coalesce on free all

---

### workload2_strategies.txt
**Purpose:** Compare allocation strategies (First Fit, Best Fit, Worst Fit)

**Tests:**
- Identical allocation patterns with different strategies
- Hole filling behavior differences
- Fragmentation patterns
- Edge cases (exact fit, full memory)

**Key Insights:**
- First Fit: Fast, but can waste space in early holes
- Best Fit: Minimizes waste per allocation, but creates tiny fragments
- Worst Fit: Leaves bigger remaining fragments

---

### workload3_cache.txt
**Purpose:** Comprehensive cache behavior tests

**Tests:**
1. Cold start / compulsory misses
2. Temporal locality (reaccessing same data)
3. Spatial locality (accessing within same cache block)
4. L2 hit scenario (evicted from L1 but still in L2)
5. Sequential stride pattern
6. Working set analysis
7. Cache thrashing

**Note:** Uses default cache config (press Enter for all prompts)

---

### workload4_stress.txt
**Purpose:** Heavy allocation/deallocation stress test

**Tests:**
- Many small allocations
- Checkerboard free pattern
- Fragmentation under load
- Coalescing many blocks

---

### workload5_edge_cases.txt
**Purpose:** Edge cases and error handling

**Tests:**
- Operations before init
- Zero-size allocation
- Exact fit allocation
- Over-allocation
- Double free
- Free non-existent block
- Re-initialization

---

### workload6_cache_policies.txt
**Purpose:** Demonstrate LRU vs FIFO replacement policy differences

**Tests:**
- Specific access pattern to show policy differences
- Uses small cache config for clear demonstration

---

## Expected Behaviors

### Memory Allocator
- Allocations should be contiguous when possible
- Free should mark blocks as FREE
- Adjacent free blocks should coalesce
- Stats should accurately reflect memory state

### Cache Simulator
- First access to any address = MISS (compulsory)
- Repeated access to same address = HIT (if not evicted)
- Addresses within same block share cache line
- Misses cascade: L1 MISS → L2 → MEMORY
