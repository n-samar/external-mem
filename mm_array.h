#ifndef MM_ARRAY_H_
#define MM_ARRAY_H_

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <string>

namespace bip = boost::interprocess;

template <class T>
class MmArray {
 public:
  MmArray<T>(const std::string& filename, int size);
  MmArray(const MmArray&) = delete;
  MmArray& operator=(MmArray&&) = default;

  std::size_t size() const { return mapped_rgn_.get_size(); }

  T& operator[](int index) {
    assert(index < size() && index >= 0);
    return mapped_data[index];
  }

  T& push_back(const T& value);

 private:
  bip::mapped_region mapped_rgn_;
  T* mapped_data;
};

template <class T>
MmArray<T>::MmArray(const std::string& filename, int size)
    : mapped_rgn_(bip::mapped_region(
          bip::file_mapping(filename.c_str(), bip::read_write), bip::read_write,
          /*offset=*/0, /*size in bytes=*/sizeof(T) * size)),
      mapped_data(static_cast<T*>(mapped_rgn_.get_address())) {}

template <class T>
T& MmArray<T>::push_back(const T& value) {}

#endif  // MM_ARRAY_H_
