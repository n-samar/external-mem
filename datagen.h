#ifndef DATAGEN_H_
#define DATAGEN_H_

#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>

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
void GenerateData(const std::string& filename, uint64_t size) {
  std::ofstream os(filename, std::ios::binary);
  uint64_t vec_size = (1 << 26) / sizeof(T);
  std::vector<T> v(vec_size);
  for (uint64_t i = 0; i < size; i+=vec_size) {
    std::generate(v.begin(), v.end(), RandomValue<T>);
    os.write(reinterpret_cast<char*>(&v[0]), vec_size);
    std::cout << i / (1 << 20) << "MB out of " << size / (1 << 30) << "GB" << std::endl;
  }
  for (uint64_t i = 0; i < size; ++i) {
    T new_value = RandomValue<T>();
    os.write(reinterpret_cast<char*>(&new_value), vec_size*sizeof(T));
  }
}

#endif  // DATAGEN_H_
