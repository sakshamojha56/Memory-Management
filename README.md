🧠 Memory Management & Cache Simulator

A C++ command-line simulator that demonstrates core operating system memory management techniques and multi-level CPU cache behavior.
The project focuses on allocator design, fragmentation handling, and cache performance analysis.

⚠️ Educational simulation — not a real OS kernel
🎥 Demo Video: https://drive.google.com/file/d/1OpTEWTd2SoTQQTp_5MN3mp6gbxsAqJ_a/view

🚀 Key Features
🔹 Dynamic Memory Allocation

Simulates physical memory using a contiguous byte array

Supports First Fit, Best Fit, and Worst Fit strategies

Block splitting and coalescing to reduce fragmentation

Runtime strategy switching via CLI

🔹 Interactive CLI
malloc <size>        Allocate memory
free <address>      Free memory
strategy <type>     first | best | worst
stats               Memory statistics
access <var>        Cache access simulation
cachestats          Cache metrics
stress <n> <max>    Stress test allocator
dump                Export memory layout
exit                Quit simulator

🔹 Memory Metrics

Used vs free memory

Number of blocks

Largest free block

External fragmentation ratio

Allocation success/failure counts

🧠 Cache Simulation

Simulates a 3-level CPU cache hierarchy:

Level	Size
L1	32 KB
L2	256 KB
L3	8 MB

Set-associative caches

LRU replacement policy

Hit/miss tracking per level

Average Memory Access Time (AMAT)

Access flow:

CPU → L1 → L2 → L3 → Main Memory

🧪 Stress Testing & Visualization

Randomized allocation/free workloads

JSON export of memory layout

Python-based visualization for fragmentation analysis

▶️ Build & Run
g++ src/*.cpp -I include -std=c++17 -O2 -o build/mma_cli
./build/mma_cli


Run with test workload:

./build/mma_cli < tests/workloads/cache_lru.txt

⚠️ Limitations

Single-threaded

No virtual memory or paging

Fixed memory pool

Symbolic cache latencies

No alignment handling

👤 Author

Saksham Ojha

📄 License

Created for educational and academic purposes.
