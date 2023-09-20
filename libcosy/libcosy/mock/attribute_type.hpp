#ifndef mock_attribute_type_hpp_INCLUDED
#define mock_attribute_type_hpp_INCLUDED

#include <libcosy/attribute_type.hpp>

class AttributeTypeMock: public AttributeType {
public:

  AttributeTypeMock(const std::string& name, int size, int alignment)
  : AttributeType(name), size_(size), alignment_(alignment) {}

  auto size()      -> int override { return size_; }
  auto alignment() -> int override { return alignment_; }

private:

  int size_;
  int alignment_;
};

#endif // mock_attribute_type_hpp_INCLUDED
