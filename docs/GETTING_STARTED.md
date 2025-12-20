# 🚀 Getting Started Guide

Welcome to your Memory Management Simulator project! This guide will help you understand everything you need to work on this project effectively.

## Table of Contents
1. [Project Overview](#project-overview)
2. [Essential Linux Commands](#essential-linux-commands)
3. [Git Workflow for Beginners](#git-workflow-for-beginners)
4. [C++ Concepts You'll Use](#c-concepts-youll-use)
5. [Building and Running](#building-and-running)
6. [Development Workflow](#development-workflow)
7. [Troubleshooting](#troubleshooting)

---

## Project Overview

This simulator models how an operating system manages memory:

| Component | What it does | Difficulty |
|-----------|--------------|------------|
| **Memory Allocator** | Simulates malloc/free with First Fit, Best Fit, Worst Fit | Core (Required) |
| **Cache Simulator** | Simulates L1/L2 CPU caches | Core (Required) |
| **Buddy Allocator** | Power-of-2 allocation system | Optional |
| **Virtual Memory** | Page tables and address translation | Optional |

---

## Essential Linux Commands

### File Navigation
```bash
pwd                     # Print current directory
ls                      # List files
ls -la                  # List ALL files with details
cd directory            # Change directory
cd ..                   # Go up one directory
cd ~                    # Go to home directory
```

### File Operations
```bash
mkdir foldername        # Create folder
touch filename          # Create empty file
rm filename             # Delete file
rm -r foldername        # Delete folder (recursive)
cp source dest          # Copy file
mv source dest          # Move/rename file
cat filename            # Display file contents
```

### Building Code
```bash
make                    # Build project using Makefile
make clean              # Remove compiled files
make debug              # Build with debug symbols
./build/memsim          # Run the program
```

### Useful Tips
```bash
clear                   # Clear terminal screen
history                 # Show command history
Tab key                 # Auto-complete file/folder names
Ctrl+C                  # Cancel running command
Ctrl+L                  # Clear screen (alternative)
```

---

## Git Workflow for Beginners

### Understanding Git Concepts

```
Working Directory → Staging Area → Local Repository → Remote (GitHub)
     (your files)      (git add)      (git commit)       (git push)
```

### Initial Setup (One Time)
```bash
# Configure your identity
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"

# Initialize repository (already done if you clone)
git init
```

### Daily Workflow

#### 1. Check Status (Do this often!)
```bash
git status              # See what files changed
git diff                # See actual changes
```

#### 2. Stage Changes
```bash
git add filename        # Stage specific file
git add .               # Stage ALL changes
git add src/            # Stage entire folder
```

#### 3. Commit Changes
```bash
git commit -m "Add first fit allocation algorithm"
```

**Good commit messages:**
- ✅ "Add first fit allocation algorithm"
- ✅ "Fix memory coalescing bug"
- ✅ "Implement cache hit/miss tracking"
- ❌ "fix stuff"
- ❌ "asdf"
- ❌ "changes"

#### 4. Push to Remote
```bash
git push origin main    # Push to GitHub
```

#### 5. Pull Latest Changes
```bash
git pull origin main    # Get latest from GitHub
```

### Branching (Intermediate)
```bash
git branch              # List branches
git branch feature-x    # Create new branch
git checkout feature-x  # Switch to branch
git checkout -b feature-x  # Create AND switch (shortcut)
git merge feature-x     # Merge branch into current
```

### If Something Goes Wrong
```bash
git log --oneline       # See commit history
git checkout -- file    # Discard changes in file
git reset HEAD file     # Unstage a file
git stash               # Temporarily save changes
git stash pop           # Restore stashed changes
```

### Recommended Workflow for This Project
```
main (stable) ─────────────────────────────────────►
    │
    └── feature/allocator ────► merge back
    │
    └── feature/cache ────────► merge back
```

---

## C++ Concepts You'll Use

### 1. Pointers and Dynamic Memory
```cpp
// Creating objects on heap
MemoryBlock* block = new MemoryBlock(0, 1024);

// Deleting when done
delete block;

// Linked list traversal
MemoryBlock* current = head;
while (current != nullptr) {
    // process current
    current = current->next;
}
```

### 2. Classes and Objects
```cpp
class Allocator {
private:
    MemoryBlock* head;    // Private data
    
public:
    bool allocate(size_t size);  // Public methods
    void free(int id);
};
```

### 3. Enums for Type Safety
```cpp
enum class AllocationStrategy {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
};
```

### 4. Standard Library Containers
```cpp
#include <vector>
#include <list>
#include <unordered_map>

std::vector<CacheLine> lines;       // Dynamic array
std::list<size_t> fifo_queue;       // Doubly-linked list
std::unordered_map<int, Block*> map; // Hash table
```

### 5. Hexadecimal Numbers
```cpp
// Memory addresses are often shown in hex
size_t address = 0x1000;  // = 4096 in decimal
std::cout << std::hex << address;  // prints "1000"
std::cout << std::dec << address;  // prints "4096"
```

---

## Building and Running

### Quick Start
```bash
cd ~/memory-simulator
make
./build/memsim
```

### Debug Build
```bash
make clean
make debug
./build/memsim
```

### Running Test Workloads
```bash
./build/memsim < tests/workload1_basic.txt
```

### Using the Debugger (GDB)
```bash
# Build with debug symbols
make debug

# Start debugger
gdb ./build/memsim

# Inside GDB:
(gdb) break main          # Set breakpoint
(gdb) run                  # Start program
(gdb) next                 # Execute next line
(gdb) step                 # Step into function
(gdb) print variable       # Print value
(gdb) continue             # Continue execution
(gdb) quit                 # Exit GDB
```

---

## Development Workflow

### Recommended Order of Implementation

```
Week 1-2: Core Memory Allocator
├── 1. Basic memory initialization
├── 2. First Fit allocation
├── 3. Memory freeing
├── 4. Block coalescing
├── 5. Best Fit and Worst Fit
└── 6. Statistics tracking

Week 3-4: Cache Simulation
├── 1. Single-level cache
├── 2. FIFO replacement
├── 3. LRU replacement
├── 4. Multi-level hierarchy
└── 5. Statistics and hit/miss tracking

Week 5+: Optional Features
├── Buddy allocation
├── Virtual memory
└── Page replacement policies
```

### File Organization
```
memory-simulator/
├── src/
│   ├── main.cpp              # CLI and command parsing
│   ├── allocator/
│   │   └── allocator.cpp     # Memory allocation logic
│   ├── cache/
│   │   └── cache.cpp         # Cache simulation
│   ├── buddy/                # (Optional) Buddy allocator
│   └── virtual_memory/       # (Optional) VM simulation
├── include/
│   ├── allocator.h           # Allocator declarations
│   └── cache.h               # Cache declarations
├── tests/                    # Test workloads
├── docs/                     # Documentation
└── Makefile                  # Build configuration
```

---

## Troubleshooting

### Compilation Errors

**"undefined reference to..."**
- You're using a function that's declared but not defined
- Check that all .cpp files are being compiled

**"no matching function..."**
- Wrong number or type of arguments
- Check function signature in header file

**Segmentation Fault**
- Usually null pointer dereference
- Use GDB to find the exact line:
```bash
gdb ./build/memsim
(gdb) run
# Program crashes
(gdb) backtrace    # Shows where it crashed
```

### Git Problems

**"Your branch is behind..."**
```bash
git pull origin main
```

**"Merge conflict"**
1. Open the conflicting file
2. Look for `<<<<<<<` markers
3. Manually choose which code to keep
4. Remove the conflict markers
5. `git add` the file
6. `git commit`

### Make Problems

**"Nothing to be done for 'all'"**
```bash
make clean
make
```

**"No rule to make target..."**
- Check that the file exists
- Check the path in Makefile

---

## Quick Reference Card

```
┌─────────────────────────────────────────────────────────────┐
│                    QUICK REFERENCE                          │
├─────────────────────────────────────────────────────────────┤
│ BUILD:    make           make debug         make clean      │
│ RUN:      ./build/memsim                                    │
│ TEST:     ./build/memsim < tests/workload1_basic.txt        │
├─────────────────────────────────────────────────────────────┤
│ GIT:      git status → git add . → git commit -m "msg"      │
│           git push origin main                              │
├─────────────────────────────────────────────────────────────┤
│ DEBUG:    gdb ./build/memsim                                │
│           (gdb) break main → run → next → print var         │
└─────────────────────────────────────────────────────────────┘
```

Good luck with your project! 🎉
