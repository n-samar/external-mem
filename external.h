#ifndef EXTERNAL_H_
#define EXTERNAL_H_

#include <iostream>
#include <string>

#include <stxxl/sort>
#include <stxxl/map>
#include <stxxl/vector>
#include <stxxl/io>
#include <stxxl/stack>

#include "constants.h"
#include "segment.h"

struct CompareLessThan
{
 bool operator () (const double& a, const double& b) const
 { return a < b; }

 static double max_value()
 { return std::numeric_limits<double>::max(); }
};

struct PairCmp
{
 typedef std::pair<double, Segment> first_argument_type;
 typedef std::pair<double, Segment> second_argument_type;
 typedef bool result_type;
 bool operator () (const std::pair<double, Segment>& a, const std::pair<double, Segment>& b) const
 {
 return a.first < b.first;
 }

 static std::pair<double, Segment> min_value()
 {
    Segment s;
    return std::make_pair(std::numeric_limits<double>::min(), s);
 }
 static std::pair<double, Segment> max_value()
 {
  Segment s;
  return std::make_pair(std::numeric_limits<double>::max(), s);
 }
};


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
    std::ofstream os("result_external.txt");
    stxxl::syscall_file f(filename, stxxl::file::DIRECT | stxxl::file::RDWR);
    stxxl::vector<Segment, 1, stxxl::lru_pager<8>, kBlockSize> v(&f, element_count);

    std::cout << "Sorting..." << std::endl;
    stxxl::sort(v.begin(), v.end(), Cmp(), kMemorySize);

    typedef stxxl::STACK_GENERATOR<Segment>::result big_stack;
    big_stack s, s_prime;

    for (Segment segment : v) {
      if (segment.lhs.x == segment.rhs.x) {
        // Point
        double curr_x = segment.lhs.x;
        while (!s.empty()) {
            if (s.top().rhs.x > curr_x) {
              // alive
              // report intersection
              os << segment << " " << s.top() << "\n";
              s_prime.push(s.top());
            }
            s.pop();
        }
        while (!s_prime.empty()) {
            s.push(s_prime.top());
            s_prime.pop();
        }
      } else {
        // Segment
        s.push(segment);
      }
    }
}

void TwoDIntersectionExternalBTree(const std::string& filename, uint64_t element_count) {
    std::cout << "Creating vector..." << std::endl;
    std::ifstream fs(filename, std::ios::in | std::ios::binary);
    std::ofstream os("result_btree.txt", std::ios::trunc);
    std::vector<Segment> vec(element_count);
    fs.read(reinterpret_cast<char*>(vec.data()), sizeof(Segment) * element_count);

    stxxl::vector<std::pair<double, Segment>, 1, stxxl::lru_pager<8>, kBlockSize> segment_vec(element_count);

    int count = 0;
    int interval = 10000000;
    for (Segment in_segment : vec) {
        if (++count % interval) {
            std::cout << "FIRST STEP " << int(double(count)/vec.size()*100) << "% through" << std::endl;
        }
        segment_vec.push_back(std::make_pair(in_segment.lhs.y, in_segment));
        if (in_segment.lhs.x == in_segment.rhs.x) {
          segment_vec.push_back(std::make_pair(in_segment.rhs.y, in_segment));
        }
    }

    stxxl::sort(segment_vec.begin(), segment_vec.end(), PairCmp(), kMemorySize);

    typedef stxxl::map<double, Segment, CompareLessThan, kBlockSize, kBlockSize> map_type;
    map_type y_map(10000*map_type::node_block_type::raw_size,
            10000*map_type::node_block_type::raw_size);

    count = 0;
    for (const auto& [y_coord, segment] : segment_vec) {
        y_map[y_coord] = segment;
        if (++count % interval) {
            std::cout << "SECOND STEP " << int(double(count)/segment_vec.size()*100) << "% through" << std::endl;
        }
        y_map[segment.lhs.y] = segment;
        if (segment.lhs.x == segment.rhs.x) {
          y_map[segment.rhs.y] = segment;
        }
    }

    map_type v_map(1000*map_type::node_block_type::raw_size,
            1000*map_type::node_block_type::raw_size);
    count = 0;
    for (const auto& [ y_coord, segment ] : y_map) {
        if (++count % interval) {
            std::cout << "THIRD STEP " << int(double(count)/y_map.size()*100) << "% through" << std::endl;
        }
        if (y_coord == segment.lhs.y && y_coord < segment.rhs.y) {
            // new v-segment
            v_map[segment.lhs.x] = segment;
        } else if (y_coord == segment.rhs.y && y_coord > segment.lhs.y) {
            // old v-segment dies
            v_map.erase(segment.lhs.x);
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

#endif  // EXTERNAL_H_
