#ifndef actor_type_hpp_INCLUDED
#define actor_type_hpp_INCLUDED

#include <libcosy/record_type.hpp>

#include <string>

class ActorType: public RecordType {
public:

  const std::string name;

  ActorType(const std::string& n, RecordType&& rec_type):
    name(n),
    RecordType(std::move(rec_type)) {}
};

#endif // actor_type_hpp_INCLUDED
