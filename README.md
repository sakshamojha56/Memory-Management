# Memory Management Simulator

A C++-based simulator that models key operating system memory management concepts including dynamic memory allocation, fragmentation handling, multilevel CPU cache behavior with LRU replacement, and a CLI-driven testing interface.

> **Note**: This project is a simulation, not a real OS kernel, designed to demonstrate algorithmic correctness and systems-level design.
> **Video Drive Link**: [Watch Video](https://drive.google.com/file/d/1OpTEWTd2SoTQQTp_5MN3mp6gbxsAqJ_a/view?usp=sharing)


---

## 📌 Features Implemented

### 1. Physical Memory Simulation

- Simulates a contiguous block of physical memory using a fixed-size byte array
- Memory is divided dynamically into blocks at runtime
- Each block contains metadata:
  - Block size
  - Free/used status
  - Pointer to next block (free list)

### 2. Memory Allocation Strategies

The simulator supports three allocation algorithms:

- **First Fit**: Allocates the first available block that fits
- **Best Fit**: Allocates the smallest block that fits
- **Worst Fit**: Allocates the largest available block

**Implementation Details:**
- Allocation searches the free list according to the selected strategy
- Blocks are split if they are larger than the requested size
- On deallocation, adjacent free blocks are coalesced to reduce external fragmentation
- Strategy can be changed at runtime via the CLI

### 3. Interactive Command-Line Interface

```bash
malloc <size>       # Allocate memory
free <ptr>          # Free memory by address
strategy <type>     # Set strategy: first | best | worst
stats               # Print memory statistics
access <var>        # Simulate cache access
cachestats          # Print cache statistics
stress <n> <max>    # Run randomized stress test
dump                # Export memory layout to JSON
exit                # Exit simulator
```

### 4. Metrics and Statistics

The simulator tracks and reports:

- Total blocks (used + free)
- Used blocks / Free blocks
- Total used memory
- Total free memory
- Largest free block
- External fragmentation ratio
- Allocation success/failure counts

### 5. Multilevel Cache Simulation

A three-level cache hierarchy is simulated:

| Level | Size   | Description |
|-------|--------|-------------|
| L1    | 32 KB  | Fastest, closest to CPU |
| L2    | 256 KB | Intermediate |
| L3    | 8 MB   | Largest, shared cache |

**Cache Properties:**
- Configurable cache size, block size, and associativity
- Set-associative architecture
- **LRU (Least Recently Used)** replacement policy
  - Implemented using a list per cache set
  - Most recently used at front, least recently used at back

**Cache Statistics:**
- Hits and misses per level
- Hit rate per level
- Cache access latency per level
- Average Memory Access Time (AMAT)
- Miss propagation tracking to lower levels

### 6. Cache Access Simulation

Memory accesses are simulated using the `access <var>` command.

**Access Flow:**
```
L1 → L2 → L3 → Main Memory
```

Cache misses propagate to lower levels and update statistics accordingly.

### 7. Stress Testing

Built-in stress test validates allocator stability:

```bash
stress <operations> <max_allocation_size>
```

Generates randomized allocation/free patterns to test:
- Allocator stability under load
- Fragmentation handling
- Cache correctness

### 8. Memory Visualization

Export memory layout to JSON:

```bash
dump
```

A Python visualization script (`visualizer/plot.py`) renders:
- Used blocks (visual representation)
- Free blocks (visual representation)
- Fragmentation patterns


## 🧱 Memory Layout Assumptions

- Memory is modeled as a flat contiguous byte array
- All allocations are byte-granular
- Alignment is not explicitly handled (per project clarification)
- Block metadata is stored inline before user memory
- Free list maintains pointers to available blocks

---

## 🔄 Address Translation Flow

Since virtual memory is not implemented, the address flow is simplified:

```
Simulated Address → Cache (L1 → L2 → L3) → Physical Memory
```

This keeps caching logic independent and modular.

---

## 🧪 Test Artifacts

Included test suite structure:

```
tests/
├── workloads/      # Input scripts for CLI
└── logs/           # Output logs from executions
```

**Example Usage:**

```bash
./build/mma_cli < tests/workloads/cache_lru.txt > tests/logs/cache_lru.log
```

---

## ▶️ Build & Run

### Compile

```bash
cd ~/projects/MMA
g++ src/*.cpp -I include -std=c++17 -O2 -o build/mma_cli
```

### Run

```bash
./build/mma_cli
```

### Run with Test Workload

```bash
./build/mma_cli < tests/workloads/cache_lru.txt
```

---

## ⚠️ Limitations & Simplifications

- Single-threaded simulation
- No real hardware timing measurements
- No actual OS page tables
- Cache latencies are symbolic (cycle counts, not nanoseconds)
- No thread safety or concurrency handling
- Fixed memory pool size at compile time

---

## 📚 References

- **Operating System Concepts** — Silberschatz, Galvin, Gagne
- **Modern Operating Systems** — Andrew S. Tanenbaum
- GeeksForGeeks OS articles and tutorials
- Course-provided project specification

---

## ✅ Project Summary

This simulator successfully demonstrates:

- ✓ Correct dynamic memory allocation with multiple strategies
- ✓ Fragmentation handling through coalescing
- ✓ Multiple allocation strategies (First, Best, Worst Fit)
- ✓ Multilevel cache behavior with LRU replacement
- ✓ CLI-based testing and interaction
- ✓ Performance metrics and visualization support

**Project Status**: All mandatory requirements fulfilled. Optional features documented as excluded per specification.

---

## 📝 License

This project is created for educational purposes as part of an operating systems course.

---

## 👤 Author

Tanmay Sharma

---

## 🤝 Acknowledgments

Thanks to course instructors and teaching assistants for project guidance and clarifications.
