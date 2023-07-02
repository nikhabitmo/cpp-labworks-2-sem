#include "lib/ID3V2.4parser.h"

int main() {
  std::filesystem::path file;

  std::cin >> file;

  MP3::ID3V2_4Parser parser;

  parser.parse(file);

  std::cout << parser;

  while (true) {
    if (getch() == 27) {
      return 0;
    }
  }
}