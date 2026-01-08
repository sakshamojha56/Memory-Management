#include "../include/MemoryManager.hpp"
#include "../include/MultiLevelCache.hpp"
#include <cstdint>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>

void executeStressTest(CoreAllocator &ram,
                       std::unordered_map<std::string, void *> &pointerMap,
                       int iterations, int maxAllocSize,
                       bool saveSnapshots = false) {
  std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<int> actionDist(0, 1); // 0 = alloc, 1 = free
  std::uniform_int_distribution<int> sizeDist(1, maxAllocSize);

  for (int i = 0; i < iterations; i++) {
    // Decide action: alloc if empty or random choice
    if (pointerMap.empty() || actionDist(rng) == 0) {
      size_t sz = sizeDist(rng);
      void *ptr = ram.mallocBytes(sz);

      if (ptr) {
        std::string varName = "auto_v" + std::to_string(i);
        pointerMap[varName] = ptr;
        std::cout << "[Stress] Alloc: " << sz << "B -> " << varName << "\n";
      }
    } else {
      // Random free
      auto it = pointerMap.begin();
      std::advance(it, rng() % pointerMap.size());
      ram.freeBytes(it->second);
      std::cout << "[Stress] Free: " << it->first << "\n";
      pointerMap.erase(it);
    }

    if (saveSnapshots) {
      ram.exportSnapshot("frames/data_" + std::to_string(i) + ".json");
    }
  }
}

int main() {
  CoreAllocator ram;
  CacheHierarchy cacheSystem;
  std::string inputLine;
  std::unordered_map<std::string, void *> pointerMap;

  std::cout << ">>> RAM Simulation Console <<<\n";
  std::cout << "Available Ops: malloc, free, stats, dump, vars, access, "
               "strategy, help, exit\n";

  while (true) {
    std::cout << "cmd> ";
    if (!std::getline(std::cin, inputLine))
      break;

    std::stringstream ss(inputLine);
    std::string command;
    ss >> command;

    if (command == "malloc") {
      size_t bytes;
      ss >> bytes;
      void *p = ram.mallocBytes(bytes);
      if (!p) {
        std::cout << "[!] Alloc failed.\n";
        continue;
      }
      std::string varName;
      std::cout << "Assign variable name: ";
      std::cin >> varName;
      pointerMap[varName] = p;
      std::cout << "[+] " << varName << " pointing to " << p << "\n";
    } else if (command == "free") {
      std::string key;
      ss >> key;

      if (pointerMap.find(key) == pointerMap.end()) {
        std::cout << "[!] Unknown variable: " << key << "\n";
        continue;
      }

      void *p = pointerMap[key];
      ram.freeBytes(p);
      pointerMap.erase(key);

      std::cout << "[+] Released " << key << "\n";
    }

    else if (command == "stats") {
      ram.displayStatus(pointerMap);
    } else if (command == "dump") {
      ram.exportSnapshot("memory_dump.json");
      std::cout << "[+] Snapshot saved.\n";
    } else if (command == "help") {
      std::cout << "  malloc <n>      Alloc n bytes\n"
                   "  free <name>     Free variable\n"
                   "  stats           Show usage report\n"
                   "  dump            Save JSON snapshot\n"
                   "  exit            Terminate\n"
                   "  vars            List variables\n"
                   "  strategy <mode> Set placement (first|best|worst)\n";

    } else if (command == "vars") {
      std::cout << "--- Active Pointers ---\n";
      for (auto &pair : pointerMap)
        std::cout << "  " << pair.first << " : " << pair.second << "\n";
    } else if (command == "access") {
      std::string var;
      ss >> var;

      if (pointerMap.find(var) == pointerMap.end()) {
        std::cout << "[!] Unknown var " << var << "\n";
      } else {
        uintptr_t addr = (uintptr_t)pointerMap[var];
        bool isHit = cacheSystem.access(addr);
        std::cout << (isHit ? "[CACHE HIT] " : "[CACHE MISS] ") << var << " @ "
                  << (void *)addr << "\n";
      }
    } else if (command == "cachestats") {
      cacheSystem.showReport();
    }

    else if (command == "exit") {
      std::cout << "[Cleanup] Releasing all resources... \n";

      for (auto &pair : pointerMap) {
        if (pair.second != nullptr) {
          ram.freeBytes(pair.second);
        }
      }
      pointerMap.clear();
      std::cout << "[Cleanup] Complete. Bye.\n";
      break;
    } else if (command == "strategy") {
      std::string mode;
      ss >> mode;

      if (mode == "first") {
        ram.setPolicy(CoreAllocator::PlacementPolicy::FIRST_FIT);
        std::cout << "[Config] Policy: First-Fit\n";
      } else if (mode == "best") {
        ram.setPolicy(CoreAllocator::PlacementPolicy::BEST_FIT);
        std::cout << "[Config] Policy: Best-Fit\n";
      } else if (mode == "worst") {
        ram.setPolicy(CoreAllocator::PlacementPolicy::WORST_FIT);
        std::cout << "[Config] Policy: Worst-Fit\n";
      } else {
        std::cout << "[!] Invalid policy. Use first, best, or worst.\n";
      }
    } else if (command == "stress") {
      int n, maxSz;
      ss >> n >> maxSz;
      executeStressTest(ram, pointerMap, n, maxSz, true);
      std::cout << "[+] Stress test finished.\n";
    }
  }
  return 0;
}
