# Memory Management Simulator

A comprehensive memory management simulator implementing OS-level memory concepts.

## Features

- **Physical Memory Allocation**: First Fit, Best Fit, Worst Fit algorithms
- **Cache Simulation**: L1/L2 multilevel cache with FIFO/LRU replacement
- **Statistics**: Fragmentation metrics, hit/miss ratios
- **Optional**: Buddy allocation, Virtual memory with paging

## Building the Project

```bash
# Simple build
make

# Debug build (with debugging symbols)
make debug

# Clean build files
make clean
```

## Running

```bash
./build/memsim
```

## Project Structure

```
memory-simulator/
├── src/                    # Source files
│   ├── main.cpp           # Entry point and CLI
│   ├── allocator/         # Memory allocation algorithms
│   ├── cache/             # Cache simulation
│   ├── buddy/             # Buddy allocation (optional)
│   └── virtual_memory/    # Virtual memory (optional)
├── include/               # Header files
├── tests/                 # Test files and workloads
├── docs/                  # Documentation
├── build/                 # Compiled binaries (generated)
└── Makefile              # Build configuration
```

## Commands (CLI Interface)

```
init memory <size>         - Initialize memory of given size
set allocator <strategy>   - Set allocation strategy (first_fit/best_fit/worst_fit)
malloc <size>              - Allocate memory block
free <id>                  - Free memory block by ID
dump memory                - Show memory state
stats                      - Show statistics
help                       - Show available commands
exit                       - Exit simulator
```

## Author

Dhruv

## License

MIT
