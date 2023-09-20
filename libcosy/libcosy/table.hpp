#ifndef table_hpp_INCLUDED
#define table_hpp_INCLUDED

#include <libcosy/actor_type.hpp>
#include <libcosy/basic_types.hpp>

#include <limits>

class Table {
public:

  Table(ActorType* type):
    type_(type),
    available_ids_({std::make_pair(1, std::numeric_limits<ActorId>::max())}) {}

  auto newActor() ->ActorId;
  auto newActors(int) ->ActorSet;
  void deleteActor(ActorId);
  void deleteActors(const ActorSet&);

  auto numActors() ->uint64_t;

private:

  void resizeBuffer_();

  ActorType*        type_;
  ActorSet          available_ids_;
  std::vector<char> buffer_;
  uint64_t          num_actors_ = 0;
};

#endif // table_hpp_INCLUDED
