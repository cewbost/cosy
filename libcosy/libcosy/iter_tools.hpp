#ifndef iter_tools_hpp_INCLUDED
#define iter_tools_hpp_INCLUDED

#include <concepts>
#include <utility>

namespace itertools {

  template<class Fn>
  class invoker {
  public:

    using difference_type = std::ptrdiff_t;

    invoker(Fn fn): fn_(fn) {}

    invoker(invoker&& other): fn_(std::move(other.fn_)) {}
    auto operator = (invoker&& other) ->invoker& {
      fn_ = std::move(other.fn_);
      return *this;
    }

    template<class T> requires std::invocable<Fn, T>
    auto operator = (const T& t) ->invoker& {
      fn_(t);
      return *this;
    }

    auto operator *  ()    ->invoker& { return *this; }
    auto operator ++ ()    ->invoker& { return *this; }
    auto operator ++ (int) ->invoker& { return *this; }

  private:

    Fn fn_;
  };
}

#endif // iter_tools_hpp_INCLUDED
