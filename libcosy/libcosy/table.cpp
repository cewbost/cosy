#include <libcosy/table.hpp>

#include <cassert>

namespace {
  auto smallestGreaterPow2_(uint64_t n) ->uint64_t {
    uint64_t res = 1;
    while (res < n) res <<= 1;
    return res;
  }
}

auto Table::newActor() ->ActorId {
  ++num_actors_;
  resizeBuffer_();
  return available_ids_.takeHead();
}

auto Table::newActors(int n) ->ActorSet {
  num_actors_ += n;
  resizeBuffer_();
  return available_ids_.takeHead(n);
}

void Table::deleteActor(ActorId id) {
  assert(!available_ids_.contains(id));
  --num_actors_;
  available_ids_.insert(id);
}

void Table::deleteActors(const ActorSet& ids) {
  assert(available_ids_.intersect(ids).size() == 0);
  num_actors_ -= ids.size();
  available_ids_.merge(ids);
}

auto Table::numActors() ->uint64_t {
  return num_actors_;
}

void Table::resizeBuffer_() {
  if (type_->size() > 0 && num_actors_ > buffer_.size() / type_->size()) {
    buffer_.resize(smallestGreaterPow2_(num_actors_) * type_->size());
  }
}
