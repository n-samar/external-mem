#ifndef MM_ARRAY_H_
#define MM_ARRAY_H_

#include <atomic>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

constexpr int kThreshold = 1 << 30;  // 1GB

namespace bip = boost::interprocess;

template <class T>
class MmArray {
 public:
  MmArray<T>(uint64_t size)
      : is_optimized_(true),
        optimization_(size),
        block_size_(0),
        mapped_rgn_(),
        mapped_data() {
    // nsamar: only allow vector optimization if size is less than kThreshold
    // bytes
    assert(size * sizeof(T) < kThreshold);
  }
  MmArray<T>(const std::string& filename, uint64_t block_size = 1);
  MmArray(const MmArray&) = delete;
  MmArray(MmArray&&) = default;
  MmArray& operator=(MmArray&&) = default;

  uint64_t size() const {
    if (is_optimized_) {
      return optimization_.size();
    }
    return mapped_rgn_.get_size() / sizeof(T);
  }

  T& operator[](uint64_t index) {
    assert(index < size() && index >= 0);
    if (is_optimized_) {
      return optimization_[index];
    }
    return mapped_data[index];
  }

  const T& operator[](uint64_t index) const {
    assert(index < size() && index >= 0);
    if (is_optimized_) {
      return optimization_[index];
    }
    return mapped_data[index];
  }

  uint64_t BlockSize() const { return block_size_; }

 private:
  bool is_optimized_;
  std::vector<T> optimization_;
  uint64_t block_size_;
  bip::mapped_region mapped_rgn_;
  T* mapped_data;
};

template <class T>
MmArray<T> MakeAnonymousMmArray(uint64_t size, uint64_t block_size = 1) {
  static std::atomic<uint64_t> tmp_index(0);
  if (size * sizeof(T) < kThreshold) {
    return MmArray<T>(size);
  } else {
    std::string filename = "tmp" + std::to_string(++tmp_index);
    std::ofstream ofs(filename, std::ios::binary | std::ios::out);
    ofs.seekp(sizeof(T) * size);
    ofs.write("", 1);
    ofs.close();
    std::cout << filename << std::endl;
    return std::move(MmArray<T>(filename, block_size));
  }
}

template <class T>
MmArray<T>::MmArray(const std::string& filename, uint64_t block_size)
    : is_optimized_(false),
      optimization_(),
      block_size_(block_size),
      mapped_rgn_(bip::mapped_region(
          bip::file_mapping(filename.c_str(), bip::read_write), bip::read_write,
          /*offset=*/0)),
      mapped_data(static_cast<T*>(mapped_rgn_.get_address())) {}

template <class T>
void Merge(MmArray<T>& array, uint64_t low, uint64_t high, uint64_t mid) {
  assert(low < mid);
  assert(mid < high);
  uint64_t low_idx = low;
  uint64_t high_idx = mid;
  auto merged = MakeAnonymousMmArray<T>(high - low, array.BlockSize());
  uint64_t curr_idx = 0;
  while (curr_idx < high - low) {
    if (low_idx == mid) {
      merged[curr_idx++] = array[high_idx];
    } else if (high_idx == high) {
      merged[curr_idx++] = array[low_idx];
    } else {
      merged[curr_idx++] = std::min(array[low_idx], array[high_idx]);
    }
    if (high_idx == high ||
        (low_idx < mid && array[low_idx] < array[high_idx])) {
      ++low_idx;
    } else if (high_idx < high) {
      ++high_idx;
    }
  }
  for (uint64_t idx = 0; idx < high - low; ++idx) {
    array[low + idx] = merged[idx];
  }
}

template <class T>
void SortHelper(MmArray<T>& array, uint64_t low, uint64_t high) {
  assert(low < high);
  if (high - low == 1) {
    return;
  }
  uint64_t mid = (low + high) / 2;
  SortHelper(array, low, mid);
  SortHelper(array, mid, high);
  Merge(array, low, high, mid);
}

template <class T>
void Sort(MmArray<T>& array) {
  SortHelper(array, 0, array.size());
}

template <class T>
void ExternalMemorySort(MmArray<T>& array) {
  // TODO(nsamar)
}

#endif  // MM_ARRAY_H_
