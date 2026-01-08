#include "../include/MultiLevelCache.hpp"
#include <iomanip>
#include <iostream>


CacheHierarchy::CacheHierarchy() {
  // Config: Name, Size, BlockSize, Cycles
  layers.emplace_back("L1 Cache", 32 * 1024, 64, 1);
  layers.emplace_back("L2 Cache", 256 * 1024, 64, 5);
  layers.emplace_back("L3 Cache", 8 * 1024 * 1024, 64, 20);
}

bool CacheHierarchy::access(uintptr_t addr) {
  accessCount++;

  // Check each layer sequentially
  for (auto &layer : layers) {
    totalCycles += layer.latency;

    if (layer.access(addr)) {
      return true; // Hit
    }
  }

  // If we reach here, it's a miss in all caches -> Main RAM access
  totalCycles += mainMemDelay;
  return false;
}

void CacheHierarchy::showReport() const {
  std::cout << "\n=== Cache Performance Report ===\n";
  std::cout << std::fixed << std::setprecision(2);

  for (const auto &layer : layers) {
    size_t requests = layer.hits + layer.misses;
    double rate =
        (requests > 0) ? ((double)layer.hits / requests * 100.0) : 0.0;

    std::cout << " [" << layer.name << "]"
              << " Requests: " << requests << " | Hits: " << layer.hits
              << " | Misses: " << layer.misses << " | Hit Rate: " << rate
              << " %\n";
  }

  double avgCycles =
      (accessCount > 0) ? (double)totalCycles / accessCount : 0.0;

  std::cout << "--------------------------------\n";
  std::cout << " Total Lookups: " << accessCount << "\n";
  std::cout << " Avg Access Time (AMAT): " << avgCycles << " cycles\n";
  std::cout << "--------------------------------\n";
}
