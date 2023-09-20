#ifndef record_type_hpp_INCLUDED
#define record_type_hpp_INCLUDED

#include <libcosy/attribute.hpp>

#include <vector>
#include <unordered_set>
#include <stdexcept>
#include <format>
#include <algorithm>

namespace ranges = std::ranges;

class RecordType {
public:

  RecordType(std::initializer_list<Attribute> list);

  auto size() const ->size_t;

private:

  size_t size_;
  std::vector<Attribute> attributes_;
};

#endif // record_type_hpp_INCLUDED
