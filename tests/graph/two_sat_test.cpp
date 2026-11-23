// Repository: Library3372
// File: tests/graph/two_sat_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/graph/two_sat_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/graph/two_sat.hpp"

#include <cassert>
#include <random>
#include <utility>
#include <vector>

namespace {

using library3372::TwoSat;

struct Clause {
  int i;
  bool f;
  int j;
  bool g;
};

bool satisfies_all(const std::vector<bool>& ans,
                   const std::vector<Clause>& clauses) {
  for (const auto& c : clauses) {
    if ((ans[c.i] != c.f) && (ans[c.j] != c.g)) return false;
  }
  return true;
}

bool brute_force_sat(int n, const std::vector<Clause>& clauses) {
  for (int mask = 0; mask < (1 << n); ++mask) {
    bool ok = true;
    for (const auto& c : clauses) {
      bool vi = ((mask >> c.i) & 1) != 0;
      bool vj = ((mask >> c.j) & 1) != 0;
      if (vi != c.f && vj != c.g) {
        ok = false;
        break;
      }
    }
    if (ok) return true;
  }
  return false;
}

void test_empty_and_single() {
  TwoSat ts_default;
  assert(ts_default.satisfiable());
  assert(ts_default.satisfiable());
  assert(ts_default.answer().empty());

  TwoSat ts0(0);
  assert(ts0.satisfiable());
  assert(ts0.answer().empty());

  TwoSat ts1(1);
  assert(ts1.satisfiable());
  assert(ts1.answer().size() == 1);

  // Tautology (x0 = true) || (x0 = false)
  ts1.add_clause(0, true, 0, false);
  assert(ts1.satisfiable());
  assert(ts1.answer().size() == 1);

  // Unit clause forcing x0 = true
  ts1.add_clause(0, true, 0, true);
  ts1.add_clause(0, true, 0, true);
  assert(ts1.satisfiable());
  assert((ts1.answer() == std::vector<bool>{true}));

  // Contradictory unit clause forcing x0 = false
  ts1.add_clause(0, false, 0, false);
  assert(!ts1.satisfiable());
  assert(!ts1.satisfiable());

  TwoSat ts1_false(1);
  ts1_false.add_clause(0, false, 0, false);
  assert(ts1_false.satisfiable());
  assert((ts1_false.answer() == std::vector<bool>{false}));
}

void test_incremental_and_copy_move() {
  TwoSat ts(3);
  // x0 != x1, x1 == x2
  ts.add_clause(0, true, 1, true);
  ts.add_clause(0, false, 1, false);
  ts.add_clause(1, true, 2, false);
  ts.add_clause(1, false, 2, true);
  assert(ts.satisfiable());
  assert(ts.satisfiable());
  std::vector<Clause> clauses = {
      {0, true, 1, true},
      {0, false, 1, false},
      {1, true, 2, false},
      {1, false, 2, true},
  };
  assert(satisfies_all(ts.answer(), clauses));

  // Copy before adding more clauses
  TwoSat copy_ts(ts);
  TwoSat assign_ts;
  assign_ts = ts;
  TwoSat& self_ref = assign_ts;
  assign_ts = self_ref;

  // Force x0 = true => x1 = false => x2 = false
  ts.add_clause(0, true, 0, true);
  assert(ts.satisfiable());
  assert((ts.answer() == std::vector<bool>{true, false, false}));

  // Copies remain unaffected
  assert(copy_ts.satisfiable());
  assert(satisfies_all(copy_ts.answer(), clauses));
  assert(assign_ts.satisfiable());
  assert(satisfies_all(assign_ts.answer(), clauses));

  // Force x2 = true => contradiction
  ts.add_clause(2, true, 2, true);
  assert(!ts.satisfiable());

  // Original copy can take an opposite valid branch (x0 = false => x1 = true)
  copy_ts.add_clause(0, false, 0, false);
  assert(copy_ts.satisfiable());
  assert((copy_ts.answer() == std::vector<bool>{false, true, true}));

  // Move construction and move assignment
  TwoSat moved_ts(std::move(copy_ts));
  assert(moved_ts.satisfiable());
  assert((moved_ts.answer() == std::vector<bool>{false, true, true}));
  TwoSat move_assign_ts;
  move_assign_ts = std::move(moved_ts);
  assert(move_assign_ts.satisfiable());
  assert((move_assign_ts.answer() == std::vector<bool>{false, true, true}));

  // Self-move-assignment and reuse after move
  TwoSat& move_self_ref = move_assign_ts;
  move_assign_ts = std::move(move_self_ref);
  move_assign_ts = assign_ts;
  assert(move_assign_ts.satisfiable());
  assert(satisfies_all(move_assign_ts.answer(), clauses));

  moved_ts = TwoSat(2);
  moved_ts.add_clause(0, true, 1, false);
  moved_ts.add_clause(0, false, 0, false);
  assert(moved_ts.satisfiable());
  assert((moved_ts.answer() == std::vector<bool>{false, false}));
}

void test_special_shapes() {
  // All 4 clauses on 2 variables (unsatisfiable)
  {
    TwoSat ts(2);
    ts.add_clause(0, true, 1, true);
    ts.add_clause(0, true, 1, false);
    ts.add_clause(0, false, 1, true);
    ts.add_clause(0, false, 1, false);
    assert(!ts.satisfiable());
  }

  // Long implication chain forced true from the start
  {
    constexpr int n = 20000;
    TwoSat ts(n);
    std::vector<Clause> clauses;
    clauses.reserve(n);
    ts.add_clause(0, true, 0, true);
    clauses.push_back({0, true, 0, true});
    for (int i = 0; i + 1 < n; ++i) {
      ts.add_clause(i, false, i + 1, true);
      clauses.push_back({i, false, i + 1, true});
    }
    assert(ts.satisfiable());
    const auto& ans = ts.answer();
    assert((int)ans.size() == n);
    for (int i = 0; i < n; ++i) assert(ans[i]);
    assert(satisfies_all(ans, clauses));
  }

  // Long implication chain forced false from the end
  {
    constexpr int n = 20000;
    TwoSat ts(n);
    for (int i = 0; i + 1 < n; ++i) ts.add_clause(i, false, i + 1, true);
    ts.add_clause(n - 1, false, n - 1, false);
    assert(ts.satisfiable());
    const auto& ans = ts.answer();
    assert((int)ans.size() == n);
    for (int i = 0; i < n; ++i) assert(!ans[i]);
  }

  // Contradictory cycle of equivalences with odd negations
  {
    constexpr int n = 10000;
    TwoSat ts(n);
    for (int i = 0; i + 1 < n; ++i) {
      ts.add_clause(i, false, i + 1, true);
      ts.add_clause(i, true, i + 1, false);
    }
    ts.add_clause(n - 1, false, 0, false);
    ts.add_clause(n - 1, true, 0, true);
    assert(!ts.satisfiable());
  }
}

void test_random_small(std::mt19937_64& rng, int n, int m) {
  TwoSat ts(n);
  std::vector<Clause> clauses;
  clauses.reserve(m);
  std::uniform_int_distribution<int> vdist(0, n - 1);
  std::uniform_int_distribution<int> bdist(0, 1);

  for (int k = 0; k < m; ++k) {
    int i = vdist(rng);
    bool f = bdist(rng) != 0;
    int j = vdist(rng);
    bool g = bdist(rng) != 0;
    ts.add_clause(i, f, j, g);
    clauses.push_back({i, f, j, g});
  }

  bool expected = brute_force_sat(n, clauses);
  bool actual = ts.satisfiable();
  assert(actual == expected);
  if (actual) {
    const auto& ans = ts.answer();
    assert((int)ans.size() == n);
    assert(satisfies_all(ans, clauses));
  }
}

void test_random_planted(std::mt19937_64& rng, int n, int m) {
  std::uniform_int_distribution<int> vdist(0, n - 1);
  std::uniform_int_distribution<int> bdist(0, 1);

  std::vector<bool> planted(n);
  for (int i = 0; i < n; ++i) planted[i] = bdist(rng) != 0;

  TwoSat ts(n);
  std::vector<Clause> clauses;
  clauses.reserve(m);

  for (int k = 0; k < m; ++k) {
    int i = vdist(rng);
    int j = vdist(rng);
    bool f = bdist(rng) != 0;
    bool g = bdist(rng) != 0;
    if (planted[i] != f && planted[j] != g) {
      if (bdist(rng) != 0) {
        f = planted[i];
      } else {
        g = planted[j];
      }
    }
    ts.add_clause(i, f, j, g);
    clauses.push_back({i, f, j, g});
  }

  assert(ts.satisfiable());
  const auto& ans = ts.answer();
  assert((int)ans.size() == n);
  assert(satisfies_all(ans, clauses));
}

}  // namespace

int main() {
  test_empty_and_single();
  test_incremental_and_copy_move();
  test_special_shapes();

  std::mt19937_64 rng(0x9e3779b97f4a7c15ULL);
  for (int n : {1, 2, 3, 5, 8, 12}) {
    for (int m_mult : {0, 1, 2, 3, 5}) {
      for (int trial = 0; trial < 10; ++trial) {
        test_random_small(rng, n, n * m_mult);
      }
    }
  }

  for (int n : {100, 500, 2000}) {
    for (int m_mult : {1, 2, 4, 8}) {
      for (int trial = 0; trial < 3; ++trial) {
        test_random_planted(rng, n, n * m_mult);
      }
    }
  }
  return 0;
}
