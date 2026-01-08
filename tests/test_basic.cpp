#include "../include/MemoryManager.hpp"
#include <cassert>
#include <iostream>

#define CHECK(cond, msg) \
    do{ \
        if(!(cond)){ \
            std::cerr<<"[FAIL] "<<msg<<std::endl; \
            assert(cond); \
        }else { \
            std::cout << "[PASS] "<<msg << std::endl; \
        } \
    }while(0)

    int main() {
    std::cout << "\n===== Running Basic MemoryManager Tests =====\n";

    MemoryManager mm;

    // ---------- Test 1: Single allocation ----------
    {
        void* p1 = mm.allocate(128);
        CHECK(p1 != nullptr, "Single allocation returns non-null pointer");

        mm.freeMem(p1);
        CHECK(true, "Free after single allocation does not crash");
    }

    // ---------- Test 2: Multiple allocations ----------
    {
        void* a = mm.allocate(100);
        void* b = mm.allocate(200);

        CHECK(a != nullptr && b != nullptr, "Multiple allocations succeed");

        mm.freeMem(a);
        mm.freeMem(b);
    }

    // ---------- Test 3: allocate + free + reallocate ----------
    {
        void* p1 = mm.allocate(256);
        mm.freeMem(p1);
        void* p2 = mm.allocate(256);

        CHECK(p2 != nullptr, "Reallocation of same size succeeds");
        CHECK(p1 == p2, "Allocator reuses freed block when possible");

        mm.freeMem(p2);
    }

    // ---------- Test 4: Fragmentation check after coalesce ----------
    {
        void* a = mm.allocate(100);
        void* b = mm.allocate(200);
        void* c = mm.allocate(50);

        mm.freeMem(b); // create fragmentation

        MemoryManager::Stats s_frag = mm.computeStats();
        CHECK(s_frag.totalFreeBlocks >= 2, "Fragmentation creates multiple free blocks");

        mm.freeMem(a);
        mm.freeMem(c);

        MemoryManager::Stats s_coal = mm.computeStats();
        CHECK(s_coal.totalFreeBlocks == 1, "Coalescing merges blocks back to one free block");
    }

    // ---------- Test 5: Invalid pointer + double free protection ----------
    {
        void* p = mm.allocate(128);
        mm.freeMem(p);
        
        // second free should not crash (double-free check)
        mm.freeMem(p);

        // invalid pointer
        mm.freeMem((void*)0x123456);
    }

    // ---------- Test 6: Zero allocation check ----------
    {
        void* z = mm.allocate(0);
        CHECK(z == nullptr, "allocate(0) returns nullptr");
    }

    // ---------- Test 7: Near-full allocation + overflow ----------
{
    // get initial stats
    MemoryManager::Stats s_init = mm.computeStats();
    size_t maxAlloc = s_init.largestFreeBlock;

    void* big = mm.allocate(maxAlloc);
    CHECK(big != nullptr, "Near-full allocation succeeds");

    void* too_big = mm.allocate(1);
    CHECK(too_big == nullptr, "Allocation exceeding remaining space fails");

    mm.freeMem(big);
}



    std::cout << "\n===== All basic tests completed =====\n";
    return 0;
}