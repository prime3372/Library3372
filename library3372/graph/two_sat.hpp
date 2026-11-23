// Repository: Library3372
// File: library3372/graph/two_sat.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/graph/two_sat.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_GRAPH_TWO_SAT_HPP_
#define LIBRARY3372_LIBRARY3372_GRAPH_TWO_SAT_HPP_

#include <cassert>
#include <limits>
#include <vector>

#include "library3372/graph/scc_graph.hpp"

namespace library3372 {

// 2-satisfiability solver via strongly connected components
// https://en.wikipedia.org/wiki/2-satisfiability
class TwoSat {
 public:
  TwoSat() : n_(0), scc_(0), built_(false), satisfiable_(false) {}

  explicit TwoSat(int n)
      : n_(n),
        scc_(0 <= n && n <= std::numeric_limits<int>::max() / 2 ? 2 * n : 0),
        built_(false),
        satisfiable_(false) {
    assert(0 <= n && n <= std::numeric_limits<int>::max() / 2);
  }

  void add_clause(int i, bool f, int j, bool g) {
    assert(0 <= i && i < n_);
    assert(0 <= j && j < n_);
    scc_.add_edge(2 * i + (f ? 0 : 1), 2 * j + (g ? 1 : 0));
    scc_.add_edge(2 * j + (g ? 0 : 1), 2 * i + (f ? 1 : 0));
    built_ = false;
  }

  bool satisfiable() {
    if (built_) return satisfiable_;
    built_ = true;
    scc_.build_ids();
    const auto& ids = scc_.ids();
    answer_.resize(n_);
    for (int i = 0; i < n_; ++i) {
      if (ids[2 * i] == ids[2 * i + 1]) {
        satisfiable_ = false;
        return false;
      }
      answer_[i] = ids[2 * i] < ids[2 * i + 1];
    }
    satisfiable_ = true;
    return true;
  }

  const std::vector<bool>& answer() const {
    assert(built_);
    assert(satisfiable_);
    return answer_;
  }

 private:
  int n_;
  SccGraph scc_;
  std::vector<bool> answer_;
  bool built_;
  bool satisfiable_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_GRAPH_TWO_SAT_HPP_
