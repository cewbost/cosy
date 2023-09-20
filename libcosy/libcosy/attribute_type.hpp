#ifndef attribute_type_hpp_INCLUDED
#define attribute_type_hpp_INCLUDED

#include <string>

class AttributeType {
public:

  const std::string name;

  AttributeType(const std::string& n): name(n) {}

  virtual auto size()      -> int = 0;
  virtual auto alignment() -> int = 0;
};

#endif // attribute_type_hpp_INCLUDED
