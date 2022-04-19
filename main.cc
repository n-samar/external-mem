#include <cassert>
#include <iostream>
#include <string>

#include "datagen.h"
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
  assert(argc == 3);
  uint64_t size = 1 << 31;
  std::string filename = argv[1];
  GenerateData<int>(filename, size);
  MmArray<int> vec(filename);
  Sort(vec);
  AssertSorted(vec);
  bool use_external_memory_algorithms(to_bool(argv[2]));
  (void)use_external_memory_algorithms;
}
