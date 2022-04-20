#include <cassert>
#include <iostream>
#include <iterator>
#include <string>

#include <stxxl/sort>
#include <stxxl/vector>
#include <stxxl/io>

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

int main(int argc, char* argv[]) {
  assert(argc == 3);
  uint64_t element_count = 1ul << std::stoi(argv[1]);
  kMemorySize = 1 << std::stoi(argv[2]);

  std::string filename = "hello";

  std::cout << "Generating data..." << std::endl;
  GenerateData<int>(filename, element_count);

  /*
  std::cout << "Creating vector..." << std::endl;
  std::ifstream fs(filename, std::ios::in | std::ios::binary);
  std::vector<int> vec(element_count);
  fs.read(reinterpret_cast<char*>(vec.data()), sizeof(int) * element_count);

  std::cout << "Sorting..." << std::endl;
  std::sort(vec.begin(), vec.end());
  std::cout << "Checking..." << std::endl;
  AssertSorted(vec);
  */

  std::cout << "Creating vector..." << std::endl;
  stxxl::syscall_file f(filename, stxxl::file::DIRECT | stxxl::file::RDWR);
  stxxl::vector<int, 1, stxxl::lru_pager<8>, kBlockSize> v(&f);

  std::cout << "Sorting..." << std::endl;
  stxxl::sort(v.begin(), v.end(), Cmp(), kMemorySize);
}
