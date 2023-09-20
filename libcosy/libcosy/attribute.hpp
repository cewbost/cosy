#ifndef attribute_hpp_INCLUDED
#define attribute_hpp_INCLUDED

#include <libcosy/attribute_type.hpp>

#include <string>

struct Attribute {
  std::string    name;
  AttributeType* type;
  int            offset;

  static auto lessMemoryOrder(const Attribute& a, const Attribute& b) ->bool;
};

#endif // attribute_hpp_INCLUDED
