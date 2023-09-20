#ifndef integral_set_hpp_INCLUDED
#define integral_set_hpp_INCLUDED

#include <libcosy/iter_tools.hpp>

#include <concepts>
#include <ranges>
#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <format>

namespace ranges = std::ranges;
namespace views  = std::views;

template<std::integral T>
class IntegralSet {

  using Segment          = std::pair<T, T>;
  using SegmentVec       = std::vector<Segment>;
  using SegmentIt        = SegmentVec::iterator;
  using ConstSegmentSpan = std::span<const Segment>;

  SegmentVec segments_;

public:

  IntegralSet() = default;

  IntegralSet(std::initializer_list<Segment> list) {
    mergeSegments_(views::all(list));
  }
  IntegralSet(std::initializer_list<T> list) {
    mergeValues_(views::all(list));
  }

  void insert(T val) {
    mergeValue_(val);
  }

  void merge(const IntegralSet<T>& set) {
    mergeSegmentsOrdered_(set.segments_);
  }

  auto unite(const IntegralSet<T>& set) ->IntegralSet<T> {
    if      (set.empty())             return *this;
    else if (this->segments_.empty()) return set;
    auto [set1, set2, first_seg] = pickHead_(this->segments_, set.segments_, std::less{});
    SegmentVec merged;
    auto proj = [](const auto& seg) { return seg.first; };
    merged.push_back(first_seg);
    ranges::merge(
      set1, set2,
      itertools::invoker([&merged](const Segment& seg) {
        if (seg.first > merged.back().second + 1) merged.push_back(seg);
        else if (seg.second > merged.back().second) merged.back().second = seg.second;
      }),
      {}, proj, proj
    );
    return IntegralSet<T>(std::move(merged));
  }

  auto intersect(const IntegralSet<T>& set) ->IntegralSet<T> {
    if (this->segments_.empty() || set.empty()) return {};
    auto [set1, set2, last_seg] = pickHead_(this->segments_, set.segments_, std::less{});
    SegmentVec merged;
    auto proj = [](const auto& seg) { return seg.first; };
    ranges::merge(
      set1, set2,
      itertools::invoker([&merged, &last_seg](const Segment& seg) {
        if (seg.first <= last_seg.second) merged.emplace_back(
          seg.first,
          seg.second < last_seg.second? seg.second : last_seg.second
        );
        if (seg.second > last_seg.second) last_seg = seg;
      }),
      {}, proj, proj
    );
    return IntegralSet<T>(std::move(merged));
  }

  auto takeHead() ->T {
    if (segments_.size() == 0) throw std::out_of_range("IntegralSet::takeHead: no elements");
    auto& seg = segments_[0];
    auto  ret = seg.first;
    if (seg.first == seg.second) segments_.erase(segments_.begin());
    else ++seg.first;
    return ret;
  }

  auto takeHead(size_t n) ->IntegralSet<T> {
    if (n == 0) return IntegralSet<T>{};
    auto split = split_(segments_.begin(), segments_.end(), n);

    SegmentVec res;
    res.reserve(split.take_until - segments_.begin());
    std::copy(segments_.begin(), split.take_until, std::back_inserter(res));
    res.back().second = split.split_point;

    if (split.take_until != split.keep_from)
      split.keep_from->first = split.split_point + 1;
    segments_.erase(segments_.begin(), split.keep_from);

    return IntegralSet<T>(std::move(res));
  }

  auto take(size_t from, size_t num) ->IntegralSet<T> {
    if (num  == 0) return IntegralSet<T>{};
    if (from == 0) return takeHead(num);

    auto fsplit = split_(segments_.begin(), segments_.end(), from);
    if (fsplit.keep_from == segments_.end())
      return IntegralSet<T>{};
    auto fsplit_seg = *std::prev(fsplit.take_until);
    if (fsplit.take_until != fsplit.keep_from) {
      fsplit.keep_from->first = fsplit.split_point + 1;
      fsplit_seg.second = fsplit.split_point;
    }
    auto ssplit = split_(fsplit.keep_from, segments_.end(), num);

    SegmentVec res;
    res.reserve(ssplit.take_until - fsplit.keep_from);
    std::copy(fsplit.keep_from, ssplit.take_until, std::back_inserter(res));
    res.back().second = ssplit.split_point;

    if (fsplit.take_until > ssplit.keep_from) {
      auto n_first = fsplit.take_until - segments_.begin();
      segments_.resize(segments_.size() + 1);
      std::copy_backward(
        std::next(segments_.begin(), n_first - 1),
        std::prev(segments_.end()),
        segments_.end()
      );
      segments_[n_first - 1] = fsplit_seg;
      segments_[n_first].first = ssplit.split_point + 1;
    } else {
      *std::prev(fsplit.take_until) = fsplit_seg;
      if (ssplit.take_until != ssplit.keep_from)
        ssplit.keep_from->first = ssplit.split_point + 1;
      segments_.erase(fsplit.take_until, ssplit.keep_from);
    }

    return IntegralSet<T>(std::move(res));
  }

  auto empty() const ->bool {
    return segments_.empty();
  }

  auto size() const ->size_t {
    size_t res = 0;
    for (const auto& seg: segments_) res += (seg.second + 1) - seg.first;
    return res;
  }

  auto nth(size_t idx) const ->size_t {
    for (const auto& seg: segments_) {
      if (idx < (seg.second + 1) - seg.first) return seg.first + idx;
      else idx -= (seg.second + 1) - seg.first;
    }
    throw std::out_of_range("IntegralSet::nth: index out of range");
  }

  auto contains(T val) const ->bool {
    auto it = ranges::lower_bound(segments_, val, {}, [](const auto& s) { return s.second; });
    return it != segments_.end() && it->first <= val;
  }

  bool operator == (const IntegralSet<T>&) const = default;

private:

  IntegralSet(SegmentVec&& segments): segments_(segments) {}

  template<ranges::input_range R> requires std::same_as<ranges::range_value_t<R>, Segment>
  void mergeSegments_(R segments) {
    for (const auto seg: segments) {
      mergeSegment_(seg);
    }
  }

  template<ranges::input_range R> requires std::same_as<ranges::range_value_t<R>, Segment>
  void mergeSegmentsOrdered_(R segments) {
    auto it = segments_.begin();
    for (const auto seg: segments) {
      it = mergeSegment_(seg, it);
    }
  }

  auto mergeSegment_(Segment seg) {
    mergeSegment_(seg, segments_.begin());
  }

  auto mergeSegment_(Segment seg, SegmentIt from) ->SegmentIt {
    auto bit = ranges::lower_bound(
      ranges::subrange(from, segments_.end()),
      seg.first,
      {},
      [](const auto& s) { return s.second; }
    );
    decltype(bit) eit;
    if (bit == segments_.end() || bit->first > seg.second) eit = bit;
    else eit = ranges::upper_bound(
      ranges::subrange(bit, segments_.end()),
      seg.second,
      {},
      [](const auto& s) { return s.first; }
    );
    if (bit != segments_.begin() && std::prev(bit)->second >= seg.first - 1) bit = std::prev(bit);
    if (eit != segments_.end() && eit->first <= seg.second + 1) eit = std::next(eit);
    if (bit == eit) {
      auto dist = std::distance(segments_.begin(), bit);
      segments_.insert(bit, seg);
      return std::next(segments_.begin(), dist);
    }
    bit->first = bit->first < seg.first? bit->first : seg.first;
    auto lit = std::prev(eit);
    bit->second = lit->second > seg.second? lit->second : seg.second;
    auto dist = std::distance(segments_.begin(), bit);
    segments_.erase(std::next(bit), eit);
    return std::next(segments_.begin(), dist);
  }

  template<ranges::input_range R> requires std::same_as<ranges::range_value_t<R>, T>
  void mergeValues_(R values) {
    for (const auto val: values) {
      mergeValue_(val);
    }
  }

  void mergeValue_(T val) {
    auto bit = ranges::lower_bound(
      segments_,
      val,
      {},
      [](const auto& s) { return s.second; }
    ); 
    bool merge_next = bit != segments_.end() && bit->first <= val + 1;
    bool merge_prev = bit != segments_.begin() && (bit - 1)->second >= val - 1;
    if (merge_next && merge_prev) {
      (bit - 1)->second = bit->second;
      segments_.erase(bit);
    } else if (merge_next) {
      bit->first = bit->first < val? bit->first : val;
    } else if (merge_prev) {
      (bit - 1)->second = val;
    } else {
      segments_.emplace(bit, val, val);
    }
  }

  struct SplitResult_ {
    SegmentIt take_until,
              keep_from;
    T         split_point;
  };

  auto split_(SegmentIt begin, SegmentIt end, size_t n) ->SplitResult_ {
    for (auto it = begin; it < end; ++it) {
      auto seg_len = it->second - it->first + 1;
      if (seg_len > n) {
        return SplitResult_{
          .take_until  = it + 1,
          .keep_from   = it,
          .split_point = it->first + T(n) - 1,
        };
      } else if (seg_len == n) {
        return SplitResult_{
          .take_until  = it + 1,
          .keep_from   = it + 1,
          .split_point = it->second,
        };
      } else {
        n -= seg_len;
      }
    }
    return SplitResult_{
      .take_until  = end,
      .keep_from   = end,
      .split_point = segments_.empty()? 0 : (end - 1)->second,
    };
  }

  template<class Comp>
  requires std::same_as<
    typename std::invoke_result<Comp, const Segment&, const Segment&>::type,
    bool>
  auto pickHead_(
    const SegmentVec& vec1,
    const SegmentVec& vec2,
    Comp comp
  ) ->std::tuple<ConstSegmentSpan, ConstSegmentSpan, Segment> {
    if (!vec1.empty() && (vec2.empty() || comp(vec1.front(), vec2.front()))) {
      return std::make_tuple(
        std::span(vec1.begin() + 1, vec1.end()),
        std::span(vec2),
        vec1.front()
      );
    } else {
      return std::make_tuple(
        std::span(vec1),
        std::span(vec2.begin() + 1, vec2.end()),
        vec2.front()
      );
    }
  }

  template<class Y>
  friend std::ostream& operator << (std::ostream&, const IntegralSet<Y>&);
};

template<class T>
std::ostream& operator << (std::ostream& os, const IntegralSet<T>& set) {
  os << '{';
  if (!set.segments_.empty()) {
    auto subsets = set.segments_ | views::transform([](const auto& seg) {
      return std::format("{}..{}", seg.first, seg.second);
    });
    os << *subsets.begin();
    for (const auto& ss: subsets | views::drop(1)) os << ", " << ss;
  }
  os << '}';
  return os;
}

#endif // integral_set_hpp_INCLUDED
