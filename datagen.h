#ifndef DATAGEN_H_
#define DATAGEN_H_

#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>

#include "constants.h"

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

template <class T>
void GenerateData(const std::string& filename, uint64_t element_count) {
  std::ofstream os(filename, std::ios::binary);
  uint64_t vec_size = kBlockSize / sizeof(T);
  std::vector<T> v(vec_size);
  for (uint64_t i = 0; i < element_count; i+=vec_size) {
    std::generate(v.begin(), v.end(), RandomValue<T>);
    os.write(reinterpret_cast<char*>(&v[0]), vec_size);
    if (i % (1 << 29) == 0) {
      std::cout << i / (1 << 20) << "MB out of " << element_count * sizeof(T) / (1 << 30) << "GB" << std::endl;
    }
  }
  for (uint64_t i = 0; i < element_count; ++i) {
    T new_value = RandomValue<T>();
    os.write(reinterpret_cast<char*>(&new_value), sizeof(T));
  }
}

#endif  // DATAGEN_H_
