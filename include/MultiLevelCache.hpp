#pragma once
#include "Cache.hpp"
#include <cstdint>
#include <vector>


// Represents the entire memory hierarchy
class CacheHierarchy {
private:
  std::vector<CacheLevel> layers;

  size_t mainMemDelay = 100;
  size_t accessCount = 0;
  size_t totalCycles = 0;

public:
  CacheHierarchy();

  // Check if address is in any cache layer
  bool access(uintptr_t addr);
  void showReport() const;
};