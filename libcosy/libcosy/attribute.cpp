#include <libcosy/attribute.hpp>

auto Attribute::lessMemoryOrder(const Attribute& a, const Attribute& b) ->bool {
  if (auto cmp = a.type->alignment() - b.type->alignment(); cmp > 0) return true;
  else if (cmp < 0) return false;
  else return a.type->size() > a.type->size();
}

