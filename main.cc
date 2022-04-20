#include <cassert>
#include <iostream>
#include <string>

#include "datagen.h"
#include "constants.h"
#include "mm_array.h"

bool to_bool(const std::string& x) {
  assert(x == "0" || x == "1");
  return x == "1";
}

template <class T>
void AssertSorted(const MmArray<T>& arr) {
  for (uint64_t idx = 0; idx < arr.size() - 1; ++idx) {
    assert(arr[idx] <= arr[idx + 1]);
  }
}

int main(int argc, char* argv[]) {
  assert(argc == 4);
  uint64_t element_count = 1ul << std::stoi(argv[1]);
  uint64_t size = element_count * sizeof(int);
  kMemorySize = 1 << std::stoi(argv[2]);
  kBlockSize = 1 << std::stoi(argv[3]);
  kThreshold = kMemorySize >> 2;
  std::string filename = "hello";
  std::cout << "Generating data..." << std::endl;
  GenerateData<int>(filename, size);
  std::cout << "Creating vector..." << std::endl;
  MmArray<int> vec(filename);
  std::cout << "Sorting..." << std::endl;
  Sort(vec);
  std::cout << "Checking..." << std::endl;
  AssertSorted(vec);
}
