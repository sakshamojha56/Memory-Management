#include "../include/Cache.hpp"
#include <cstdint> 
#include <iostream>

CacheLevel::CacheLevel(std::string name, size_t sizeBytes,
                       size_t lineSize, size_t latency, size_t ways)
    : name(name), sizeBytes(sizeBytes), lineSize(lineSize), ways(ways), latency(latency)
    {
    numSets = (sizeBytes / lineSize)/ways;
    sets.resize(numSets);

    //Pre-fill with invalid enteries
    for(auto &set : sets){
        for(size_t i =0;i<ways;i++)
            set.lines.push_back(CacheLine{false,0});
    }
}
bool CacheLevel::access(uintptr_t address) {
    uintptr_t tag = address / lineSize;
    uintptr_t setIndex = (address / lineSize) % numSets;

    CacheSet &set = sets[setIndex];

    // Search for tag
    for (auto it = set.lines.begin(); it != set.lines.end(); ++it) {
        if (it->valid && it->tag == tag) {
            // HIT → move to front (MRU)
            hits++;
            CacheLine entry = *it;
            set.lines.erase(it);
            set.lines.push_front(entry);
            return true;
        }
    }

    // MISS → evict LRU (back) and insert new line as MRU
    misses++;
    set.lines.pop_back();
    set.lines.push_front(CacheLine{true, tag});
    return false;
}