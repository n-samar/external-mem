#ifndef DATAGEN_H_
#define DATAGEN_H_

#include <fstream>

template <class T>
T RandomValue() = delete;

template <>
inline int RandomValue<int>() {
  return rand();
}

template <class T>
void GenerateData(const std::string& filename, int size) {
  std::ofstream os(filename, std::ios::binary);
  for (int i = 0; i < size; ++i) {
    T new_value = RandomValue<T>();
    os.write(reinterpret_cast<char*>(&new_value), sizeof(int));
  }
}

#endif  // DATAGEN_H_
