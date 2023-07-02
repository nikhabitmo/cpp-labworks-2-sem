#include "lib/MemoryPoolsAllocator.h"
#include "vector"

int main() {
  std::vector<int, PoolAllocator<int>> vec;
  PoolAllocator<int> allocator;

  int* p = allocator.allocate(2);

  std::cout << "Allocated memory at: " << p << '\n' << "value:  "<< *p << '\n';

  allocator.deallocate(p, 2);

  std::cout << "Memory deallocated\n";

  return 0;
}