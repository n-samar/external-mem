#ifndef EXTERNAL_H_
#define EXTERNAL_H_

#include <iostream>
#include <string>

#include <stxxl/sort>
#include <stxxl/vector>
#include <stxxl/io>

#include "constants.h"
#include "segment.h"

struct Cmp
{
 typedef Segment first_argument_type;
 typedef Segment second_argument_type;
 typedef bool result_type;
 bool operator () (const Segment& a, const Segment& b) const
 {
 return a.lhs.x < b.lhs.x;
 }

 static Segment min_value()
 {
    Segment s;
    Point p;
    p.x = std::numeric_limits<double>::min();
    p.y = std::numeric_limits<double>::min();
    s.lhs = p;
    s.rhs = p;
    return s;
 }
 static Segment max_value()
 {
  Segment s;
  Point p;
  p.x = std::numeric_limits<double>::max();
  p.y = std::numeric_limits<double>::max();
  s.lhs = p;
  s.rhs = p;
  return s;
 }
};

void AssertSorted(const stxxl::vector<Segment, 1, stxxl::lru_pager<8>, kBlockSize>& arr) {
  std::cout << "vector size: " << arr.size() << std::endl;
  for (uint64_t idx = 0; idx + 1 < arr.size(); ++idx) {
    assert(arr[idx] <= arr[idx + 1]);
  }
}

void SortExternal(const std::string& filename, uint64_t element_count) {
  std::cout << "Creating vector..." << std::endl;
  stxxl::syscall_file f(filename, stxxl::file::DIRECT | stxxl::file::RDWR);
  stxxl::vector<Segment, 1, stxxl::lru_pager<8>, kBlockSize> v(&f, element_count);

  std::cout << "Sorting..." << std::endl;
  stxxl::sort(v.begin(), v.end(), Cmp(), kMemorySize);
  AssertSorted(v);
}

void OneDIntersectionExternal(const std::string& filename, uint64_t element_count) {
    std::cout << "Creating vector..." << std::endl;
    stxxl::syscall_file f(filename, stxxl::file::DIRECT | stxxl::file::RDWR);
    stxxl::vector<Segment, 1, stxxl::lru_pager<8>, kBlockSize> v(&f, element_count);

    std::cout << "Sorting..." << std::endl;
    stxxl::sort(v.begin(), v.end(), Cmp(), kMemorySize);
}

#endif  // EXTERNAL_H_
