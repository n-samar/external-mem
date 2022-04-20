#include <cassert>
#include <iostream>
#include <iterator>
#include <string>

#include "datagen.h"
#include "constants.h"

bool to_bool(const std::string& x) {
  assert(x == "0" || x == "1");
  return x == "1";
}

template <class T>
void AssertSorted(const std::vector<T>& arr) {
  std::cout << "vector size: " << arr.size() << std::endl;
  for (uint64_t idx = 0; idx + 1 < arr.size(); ++idx) {
    assert(arr[idx] <= arr[idx + 1]);
  }
}

int main(int argc, char* argv[]) {
  assert(argc == 4);
  uint64_t element_count = 1ul << std::stoi(argv[1]);
  kMemorySize = 1 << std::stoi(argv[2]);
  kBlockSize = 1 << std::stoi(argv[3]);
  kThreshold = kMemorySize >> 3;

  std::string filename = "hello";

  std::cout << "Generating data..." << std::endl;
  GenerateData<int>(filename, element_count);

  std::cout << "Creating vector..." << std::endl;
  std::ifstream fs(filename, std::ios::in | std::ios::binary);
  std::vector<int> vec(element_count);
  fs.read(reinterpret_cast<char*>(vec.data()), sizeof(int) * element_count);

  std::cout << "Sorting..." << std::endl;
  std::sort(vec.begin(), vec.end());
  std::cout << "Checking..." << std::endl;
  AssertSorted(vec);
}
