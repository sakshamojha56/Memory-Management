#pragma once
#include <vector>   
#include <cstddef>
#include <string>
#include <cstdint>
#include <list>

struct CacheLine {
    bool valid = false;
    uintptr_t tag = 0;
};  

struct CacheSet {
    std::list<CacheLine> lines;  // front = MRU, back = LRU
};

class CacheLevel {
public:
    std::string name;
    size_t sizeBytes;
    size_t lineSize;
    size_t ways;
    size_t numSets;

    size_t latency;

    std::vector<CacheSet> sets;  

    size_t hits = 0;
    size_t misses = 0;

    CacheLevel(std::string name, size_t sizeBytes,
               size_t lineSize, size_t latency, size_t ways = 4);

    bool access(uintptr_t address);
};
