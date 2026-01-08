#!/bin/bash
echo "Running allocator tests..."
./build/mma_cli < tests/workloads/allocator_basic.txt > tests/logs/allocator_basic.log

echo "Running cache tests..."
./build/mma_cli < tests/workloads/cache_lru.txt > tests/logs/cache_lru.log

echo "Running stress test..."
./build/mma_cli < tests/workloads/stress.txt > tests/logs/stress.log

echo "All tests completed."
