#include <cassert>
#include <iostream>
#include <string>

#include "datagen.h"
#include "mm_array.h"

bool to_bool(const std::string& x) {
  assert(x == "0" || x == "1");
  return x == "1";
}

int main(int argc, char* argv[]) {
  assert(argc == 3);
  int size = 5;
  std::string filename = argv[1];
  GenerateData<int>(filename, size);
  MmArray<int> vec(filename, size);
  for (int i = 0; i < size; ++i) {
    std::cout << vec[i] << std::endl;
  }
  bool use_external_memory_algorithms(to_bool(argv[2]));
}
