#include <cassert>
#include <iostream>
#include <iterator>
#include <string>

#include <stxxl/stack>
#include <iostream>
#include <variant>

#include "datagen.h"
#include "main_memory.h"
#include "external.h"
#include "constants.h"
#include "segment.h"

struct CompareGreater
{
  bool operator () (const int& a, const int& b) const
  { return a > b; }
  static int max_value()
  { return std::numeric_limits<int>::min(); }
};

bool to_bool(const std::string& x) {
  assert(x == "0" || x == "1");
  return x == "1";
}

int main(int argc, char* argv[]) {
  assert(argc == 6);

  uint64_t element_count = 1ul << std::stoi(argv[1]);
  kMemorySize = 1 << std::stoi(argv[2]);

  std::string filename = "hello";

  std::cout << "Generating data..." << std::endl;
  kSegmentWidth = std::stod(argv[3]);
  bool generate_data = to_bool(argv[4]);
  if (generate_data) {
    GenerateData<Segment>(filename, element_count);
  }


  /*
  typedef stxxl::STACK_GENERATOR<int>::result stack_type;

  stack_type my_stack;

  my_stack.push(8);
  my_stack.push(7);
  my_stack.push(4);
  */

  static_assert(std::is_pod<Segment>::value);

  bool use_external_algorithms = to_bool(argv[5]);

  if (use_external_algorithms) {
    std::cout << "External sort" << std::endl;
    TwoDIntersectionExternalBTree(filename, element_count);
  } else {
    TwoDIntersectionMainMemory(filename, element_count);
  }
}
