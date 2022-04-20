#ifndef EXTERNAL_H_
#define EXTERNAL_H_

#include <iostream>
#include <string>

#include <stxxl/sort>
#include <stxxl/vector>
#include <stxxl/io>

#include "constants.h"

struct Cmp
{
 typedef int first_argument_type;
 typedef int second_argument_type;
 typedef bool result_type;
 bool operator () (const int& a, const int& b) const
 {
 return a < b;
 }
 static int min_value()
 {
   return std::numeric_limits<int>::min();
 }
 static int max_value()
 {
  return std::numeric_limits<int>::max();
 }
};

void AssertSorted(const stxxl::vector<int, 1, stxxl::lru_pager<8>, kBlockSize>& arr) {
  std::cout << "vector size: " << arr.size() << std::endl;
  for (uint64_t idx = 0; idx + 1 < arr.size(); ++idx) {
    assert(arr[idx] <= arr[idx + 1]);
  }
}

void SortExternal(const std::string& filename, uint64_t element_count) {
  std::cout << "Creating vector..." << std::endl;
  stxxl::syscall_file f(filename, stxxl::file::DIRECT | stxxl::file::RDWR);
  stxxl::vector<int, 1, stxxl::lru_pager<8>, kBlockSize> v(&f, element_count);

  std::cout << "Sorting..." << std::endl;
  stxxl::sort(v.begin(), v.end(), Cmp(), kMemorySize);
  AssertSorted(v);
}

#endif  // EXTERNAL_H_
