#include "../include/MemoryManager.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>


// #include <nlohmann/json.hpp>
// using json = nlohmann::json;

CoreAllocator::CoreAllocator() {
  // Initialize the pool
  head = reinterpret_cast<MemNode *>(poolArena);
  head->sizeBytes = POOL_SIZE - sizeof(MemNode);
  head->isFree = true;
  head->nextNode = nullptr;
}

void *CoreAllocator::mallocBytes(size_t sz) {
  if (sz == 0) {
    std::cerr << "[Core] Warning: Attempted 0-byte allocation.\n";
    return nullptr;
  }

  MemNode *candidate = nullptr;

  // Strategy Selection
  switch (currentPolicy) {
  case PlacementPolicy::FIRST_FIT: {
    MemNode *curr = head;
    while (curr) {
      if (curr->isFree && curr->sizeBytes >= sz) {
        candidate = curr;
        break;
      }
      curr = curr->nextNode;
    }
    break;
  }
  case PlacementPolicy::BEST_FIT: {
    size_t bestFit = SIZE_MAX;
    for (MemNode *curr = head; curr; curr = curr->nextNode) {
      if (curr->isFree && curr->sizeBytes >= sz && curr->sizeBytes < bestFit) {
        bestFit = curr->sizeBytes;
        candidate = curr;
      }
    }
    break;
  }
  case PlacementPolicy::WORST_FIT: {
    size_t maxSz = 0;
    for (MemNode *curr = head; curr; curr = curr->nextNode) {
      if (curr->isFree && curr->sizeBytes >= sz && curr->sizeBytes > maxSz) {
        maxSz = curr->sizeBytes;
        candidate = curr;
      }
    }
    break;
  }
  }

  if (!candidate) {
    std::cerr << "[Core] Allocation Failed: Insufficient memory (" << sz
              << " bytes requested).\n";
    return nullptr;
  }

  partitionChunk(candidate, sz);
  candidate->isFree = false;

  // Return pointer to data payload
  return reinterpret_cast<char *>(candidate) + sizeof(MemNode);
}

void CoreAllocator::freeBytes(void *ptr) {
  if (!ptr)
    return;

  if (!validateAddress(ptr)) {
    std::cerr << "[Core] Error: Invalid pointer release attempted: " << ptr
              << "\n";
    return;
  }

  MemNode *node = reinterpret_cast<MemNode *>(reinterpret_cast<char *>(ptr) -
                                              sizeof(MemNode));
  if (node->isFree) {
    std::cerr << "[Core] Error: Double-free detected at " << ptr << "\n";
    return;
  }

  node->isFree = true;
  std::cout << "[Core] Memory released at " << ptr << "\n";

  mergeFreeChunks();
}

void CoreAllocator::partitionChunk(MemNode *node, size_t needed) {
  size_t leftover = node->sizeBytes - needed;

  // Only split if enough space for another header + 1 byte
  if (leftover <= sizeof(MemNode)) {
    return;
  }

  char *splitAddr = reinterpret_cast<char *>(node) + sizeof(MemNode) + needed;
  MemNode *newMeta = reinterpret_cast<MemNode *>(splitAddr);

  newMeta->sizeBytes = leftover - sizeof(MemNode);
  newMeta->isFree = true;
  newMeta->nextNode = node->nextNode;

  node->sizeBytes = needed;
  node->nextNode = newMeta;
}

void CoreAllocator::mergeFreeChunks() {
  MemNode *curr = head;
  while (curr && curr->nextNode) {
    MemNode *next = curr->nextNode;

    char *currEnd =
        reinterpret_cast<char *>(curr) + sizeof(MemNode) + curr->sizeBytes;
    bool isAdjacent = (currEnd == reinterpret_cast<char *>(next));

    if (curr->isFree && next->isFree && isAdjacent) {
      curr->sizeBytes += (sizeof(MemNode) + next->sizeBytes);
      curr->nextNode = next->nextNode;
      // Stay on 'curr' to potentially merge with subsequent block
    } else {
      curr = curr->nextNode;
    }
  }
}

bool CoreAllocator::validateAddress(void *p) const {
  if (!p)
    return false;
  const char *start = poolArena;
  const char *end = poolArena + POOL_SIZE;
  const char *target = reinterpret_cast<const char *>(p);

  if (target < start + sizeof(MemNode) || target >= end)
    return false;

  // Optional: check alignment or strict validation of header
  return true;
}

void CoreAllocator::displayStatus(
    const std::unordered_map<std::string, void *> &activeVars) const {
  std::cout << "\n>>> System Memory State <<<\n";
  MemNode *curr = head;
  int idx = 0;

  while (curr) {
    std::string label = "[free]";
    void *userPtr = reinterpret_cast<char *>(curr) + sizeof(MemNode);

    if (!curr->isFree) {
      label = "[allocated]";
      for (const auto &kv : activeVars) {
        if (kv.second == userPtr) {
          label += " (" + kv.first + ")";
          break;
        }
      }
    }

    std::cout << "Chunk #" << idx << " | " << curr->sizeBytes << " B"
              << " | " << (curr->isFree ? "FREE" : "USED") << " | " << userPtr
              << " " << (curr->isFree ? "" : "-> " + label) << "\n";

    curr = curr->nextNode;
    idx++;
  }

  UsageStats s = getStats();
  std::cout << "----------------------------\n"
            << " Total Capacity : " << POOL_SIZE << " B\n"
            << " Used           : " << s.bytesUsed << " B ("
            << s.usedBlocksCount << " blocks)\n"
            << " Available      : " << s.bytesFree << " B ("
            << s.freeBlocksCount << " blocks)\n"
            << " Max Allocable  : " << s.maxFreeBlockSize << " B\n"
            << " Fragmentation  : " << (s.fragPercent * 100.0) << " %\n"
            << "----------------------------\n";
}

CoreAllocator::UsageStats CoreAllocator::getStats() const {
  UsageStats s;
  MemNode *curr = head;
  while (curr) {
    s.totalBlocksCount++;
    if (curr->isFree) {
      s.freeBlocksCount++;
      s.bytesFree += curr->sizeBytes;
      if (curr->sizeBytes > s.maxFreeBlockSize) {
        s.maxFreeBlockSize = curr->sizeBytes;
      }
    } else {
      s.usedBlocksCount++;
      s.bytesUsed += curr->sizeBytes;
    }
    curr = curr->nextNode;
  }

  if (s.bytesFree > 0) {
    s.fragPercent =
        1.0 - (static_cast<double>(s.maxFreeBlockSize) / s.bytesFree);
  }
  return s;
}

void CoreAllocator::exportSnapshot(const std::string &fname) const {
  std::cerr << "[Core] Snapshot feature unavailable (missing json lib).\n";
  /*
  // Implementation would go here
  */
}