#pragma once
#include <cstddef>
#include <string>
#include <unordered_map>

// Core memory allocator simulating a heap
class CoreAllocator {
public:
  struct UsageStats {
    size_t totalBlocksCount = 0;
    size_t freeBlocksCount = 0;
    size_t usedBlocksCount = 0;
    size_t bytesFree = 0;
    size_t bytesUsed = 0;
    size_t maxFreeBlockSize = 0;
    double fragPercent = 0.0;
  };

  enum class PlacementPolicy { FIRST_FIT, BEST_FIT, WORST_FIT };

private:
  // Header for each memory chunk
  struct MemNode {
    size_t sizeBytes;
    bool isFree;
    MemNode *nextNode;
  };

  bool validateAddress(void *p) const;
  static constexpr size_t POOL_SIZE = 1024 * 1024; // 1 MB
  alignas(16) char poolArena[POOL_SIZE];           // Aligned memory pool
  MemNode *head;

  void partitionChunk(MemNode *node, size_t needed);
  void mergeFreeChunks();

  PlacementPolicy currentPolicy = PlacementPolicy::FIRST_FIT;

public:
  CoreAllocator();

  // Core API
  void *mallocBytes(size_t sz);
  void freeBytes(void *ptr);

  // Diagnostics
  void displayStatus(
      const std::unordered_map<std::string, void *> &activeVars) const;
  UsageStats getStats() const;
  void exportSnapshot(const std::string &fname) const;

  void setPolicy(PlacementPolicy p) { currentPolicy = p; }
};