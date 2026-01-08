#include "../include/Allocator.hpp"
#include "../include/GlobalMemory.hpp"
#include <vector>
#include <iostream>

int main() {
    {
        std::vector<int, Allocator<int>> v;

        for (int i = 0; i < 2000; i++) {
            v.push_back(i);
        }

        std::cout << "\n=== After push_back ===\n";
        GlobalMemoryManager.printStats();

        v.clear();
        std::cout << "\n=== After clear ===\n";
        GlobalMemoryManager.printStats();
    }

    std::cout << "\n=== After vector destruction ===\n";
    GlobalMemoryManager.printStats();

    return 0;
}
