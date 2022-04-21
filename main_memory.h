#ifndef MAIN_MEMORY_H_
#define MAIN_MEMORY_H_

#include "constants.h"
#include "segment.h"

#include <vector>
#include <cassert>
#include <set>
#include <iostream>
#include <map>
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

struct cmpYdecreasing {
    bool operator()(const Point& p0, const Point& p1) const {
      return p0.y > p1.y;
    }
};

struct cmpXincreasing {
    bool operator()(const Point& p0, const Point& p1) const {
      return p0.x < p1.x;
    }
};

void TwoDIntersectionMainMemory(const std::string& filename, uint64_t element_count) {
    std::cout << "Creating vector..." << std::endl;
    std::ifstream fs(filename, std::ios::in | std::ios::binary);
    std::ofstream os("result_main_memory.txt", std::ios::trunc);
    std::vector<Segment> vec(element_count);
    fs.read(reinterpret_cast<char*>(vec.data()), sizeof(Segment) * element_count);

    std::map<double, Segment> y_map;
    for (Segment segment : vec) {
        y_map[segment.lhs.y] = segment;
        if (segment.lhs.x != segment.rhs.x) {
          y_map[segment.rhs.y] = segment;
        }
    }

    std::map<double, Segment> v_map;
    for (const auto& [ y_coord, segment ] : y_map) {
        if (y_coord == segment.lhs.y && y_coord < segment.rhs.y) {
            // new v-segment
            v_map[segment.lhs.x] = segment;
        } else if (y_coord == segment.rhs.y && y_coord > segment.lhs.y) {
            // old v-segment dies
            v_map.erase(y_coord);
        } else if (segment.lhs.y == segment.rhs.y) {
            // h-segment
            auto lb = v_map.lower_bound(segment.lhs.x);
            auto ub = v_map.upper_bound(segment.rhs.x);
            for (auto iter = lb; iter != ub; ++iter) {
                // report intersection
                os  << iter->second << " " << segment << "\n";
            }
        } else {
            assert(false);
        }
    }
}

#endif // MAIN_MEMORY_H_
