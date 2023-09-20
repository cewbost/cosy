#include <libcosy/record_type.hpp>

RecordType::RecordType(std::initializer_list<Attribute> list): attributes_(list) {
  std::unordered_set<std::string> names;
  for (const auto& attr: attributes_) {
    if (attr.name.size() == 0)
      throw std::invalid_argument("attribute must be named");
    if (!names.insert(attr.name).second)
      throw std::invalid_argument(std::format("multiple attributes named '{}'", attr.name));
  }

  size_ = 0;
  if (attributes_.size() == 0) return;
  ranges::sort(attributes_, Attribute::lessMemoryOrder);
  for (auto& attr: attributes_) {
    attr.offset = size_;
    size_ += attr.type->size();
  }
  auto alignment = attributes_[0].type->alignment();
  size_ = ((size_ + alignment - 1) / alignment) * alignment;
}

auto RecordType::size() const ->size_t {
  return size_;
}
