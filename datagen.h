#ifndef DATAGEN_H_
#define DATAGEN_H_

#include <fstream>
#include <iomanip>
#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>

#include "constants.h"
#include "segment.h"

template <class T>
T RandomValue() = delete;

template <>
inline int RandomValue<int>() {
    static int i = 0;
	int a = ++i;
    a = (a ^ 61) ^ (a >> 16);
    a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);
	return a;
}

template <>
inline uint64_t RandomValue<uint64_t>() {
    static uint64_t i = 0;
    uint64_t a = ++i;
    a ^= (a >> 33);
    a *= 0xff51afd7ed558ccdL;
    a ^= (a >> 33);
    a *= 0xc4ceb9fe1a85ec53L;
    a ^= (a >> 33);
    return a;
}

template <>
inline Segment RandomValue<Segment>() {
    assert(kVerticalRatio > 0 && kVerticalRatio < 1);
    double scale = 1 << 10;
    double width = kSegmentWidth;
    double scale_down = (uint64_t(1) << (32+31)) / scale;
    Segment result;
    if (RandomValue<uint64_t>() < kVerticalRatio * std::numeric_limits<uint64_t>::max()) {
        // v-segment
        double x0 = RandomValue<uint64_t>() / scale_down;
        double y0 = RandomValue<uint64_t>() / scale_down;
        double y1 = y0 + width;
        Point lhs;
        lhs.x = x0;
        lhs.y = std::min(y0, y1);
        Point rhs;
        rhs.x = x0;
        rhs.y = std::max(y0, y1);
        result.lhs = lhs;
        result.rhs = rhs;
    } else {
        // h-segment
        double x0 = RandomValue<uint64_t>() / scale_down;
        double x1 = x0 + width;
        double y0 = RandomValue<uint64_t>() / scale_down;
        Point lhs, rhs;
        lhs.x = std::min(x0, x1);
        lhs.y = y0;
        rhs.x = std::max(x0, x1);
        rhs.y = y0;
        result.lhs = lhs;
        result.rhs = rhs;
    }
    if (!((result.lhs.x == result.rhs.x) ^ (result.lhs.y == result.rhs.y))) {
	std::cout << result << std::endl;
	assert(false);
    }
    return result;
}

template <class T>
void GenerateData(const std::string& filename, uint64_t element_count) {
  std::ofstream os(filename, std::ios::binary);
  uint64_t vec_size = kBlockSize / sizeof(T);
  std::vector<T> v(vec_size);
  std::cout << "vec_size: " << vec_size << std::endl;
  for (uint64_t i = 0; i < element_count; i+=vec_size) {
    std::generate(v.begin(), v.end(), RandomValue<T>);
    os.write(reinterpret_cast<char*>(&v[0]), vec_size*sizeof(T));
    if (i % (1 << 29) == 0) {
      std::cout << std::setprecision(2) << static_cast<double>(i)*sizeof(T) / (1 << 30) << "GB out of " << element_count * sizeof(T) / (1 << 30) << "GB" << std::endl;
    }
  }
  std::cout << "Remaining: " << element_count % vec_size << std::endl;
  for (uint64_t i = 0; i < element_count%vec_size; ++i) {
    T new_value = RandomValue<T>();
    os.write(reinterpret_cast<char*>(&new_value), sizeof(T));
  }
}

#endif  // DATAGEN_H_
