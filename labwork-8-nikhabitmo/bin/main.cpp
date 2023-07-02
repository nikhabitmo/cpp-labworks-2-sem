#include "lib/CCircularBufferExt.h"
#include "algorithm"

int main() {
  CCircularBufferExt<int> buffer(4);
  buffer.push(5);
  buffer.push(71);
  buffer.push(2);
  buffer.push(70);
  buffer.push(8);
  buffer.push(84);
  buffer.push(55);
  buffer.push(22);

  std::sort(buffer.begin(), buffer.end());

  for (const auto& element : buffer) {
    std::cout << element << " ";
  }

  return 0;
}
