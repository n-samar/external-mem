#include <cassert>
#include <iostream>
#include <iterator>
#include <string>


#include "datagen.h"
#include "main_memory.h"
#include "external.h"
#include "constants.h"

bool to_bool(const std::string& x) {
  assert(x == "0" || x == "1");
  return x == "1";
}

int main(int argc, char* argv[]) {
  assert(argc == 4);

  uint64_t element_count = 1ul << std::stoi(argv[1]);
  kMemorySize = 1 << std::stoi(argv[2]);

  std::string filename = "hello";

  std::cout << "Generating data..." << std::endl;
  GenerateData<int>(filename, element_count);

  bool use_external_algorithms = to_bool(argv[3]);

  if (use_external_algorithms) {
    SortExternal(filename, element_count);
  } else {
    SortMainMemory(filename, element_count);
  }
}
