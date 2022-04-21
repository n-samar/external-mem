#ifndef MAIN_MEMORY_H_
#define MAIN_MEMORY_H_

#include "constants.h"
#include "segment.h"

#include <vector>
#include <set>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <iterator>

template <class T>
void AssertSorted(const std::vector<T>& arr) {
  std::cout << "vector size: " << arr.size() << std::endl;
  for (uint64_t idx = 0; idx + 1 < arr.size(); ++idx) {
    assert(arr[idx] <= arr[idx + 1]);
  }
}

void SortMainMemory(const std::string& filename, uint64_t element_count) {
    std::cout << "Creating vector..." << std::endl;
    std::ifstream fs(filename, std::ios::in | std::ios::binary);
    std::vector<Segment> vec(element_count);
    fs.read(reinterpret_cast<char*>(vec.data()), sizeof(Segment) * element_count);

    std::cout << "Sorting..." << std::endl;
    std::sort(vec.begin(), vec.end());
    std::cout << "Checking..." << std::endl;
    AssertSorted(vec);
}

void OneDIntersectionMainMemory(const std::string& filename, uint64_t element_count) {
    std::cout << "Creating vector..." << std::endl;
    std::ifstream fs(filename, std::ios::in | std::ios::binary);
    std::ofstream os("result_main_memory.txt");
    std::vector<Segment> vec(element_count);
    fs.read(reinterpret_cast<char*>(vec.data()), sizeof(Segment) * element_count);

    std::cout << "Sorting..." << std::endl;
    std::sort(vec.begin(), vec.end());
    std::cout << "Checking..." << std::endl;
    AssertSorted(vec);

    std::set<double> points;

    for (Segment segment : vec) {
      if (segment.lhs.x == segment.rhs.x) {
        // point
        points.insert(segment.lhs.x);
      }
    }
    for (Segment segment : vec) {
      if (segment.lhs.x < segment.rhs.x) {
        // segment
        auto left_iter = points.lower_bound(segment.lhs.x);
        auto right_iter = points.upper_bound(segment.rhs.x);
        for (auto iter = left_iter; iter != right_iter; ++iter) {
          // report
          os << "Point(" << *iter << ") " << segment << "\n";
        }
      }
    }
}

#endif // MAIN_MEMORY_H_
