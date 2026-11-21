// Repository: Library3372
// File: tests/graph/max_flow_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/graph/max_flow_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/graph/max_flow.hpp"

#include <algorithm>
#include <cassert>
#include <climits>
#include <queue>
#include <random>
#include <type_traits>
#include <utility>
#include <vector>

namespace {

using library3372::MaxFlow;

template <typename T>
concept can_instantiate_max_flow = requires { typename MaxFlow<T>; };

template <typename Cap> class NaiveMaxFlow {
 public:
  explicit NaiveMaxFlow(int n) : g_(n) {}

  int add_edge(int from, int to, Cap cap) {
    int m = (int)pos_.size();
    int from_id = (int)g_[from].size();
    int to_id = (int)g_[to].size();
    if (from == to) ++to_id;
    pos_.emplace_back(from, from_id);
    g_[from].push_back({to, to_id, cap});
    g_[to].push_back({from, from_id, 0});
    return m;
  }

  typename MaxFlow<Cap>::Edge get_edge(int i) const {
    const auto& e = g_[pos_[i].first][pos_[i].second];
    const auto& re = g_[e.to][e.rev];
    return {pos_[i].first, e.to, (Cap)(e.cap + re.cap), re.cap};
  }

  Cap flow(int s, int t, Cap limit) {
    int n = (int)g_.size();
    Cap total = 0;
    while (total < limit) {
      std::vector<int> pv(n, -1);
      std::vector<int> pe(n, -1);
      std::queue<int> q;
      pv[s] = s;
      q.push(s);
      while (!q.empty() && pv[t] == -1) {
        int v = q.front();
        q.pop();
        for (int i = 0; i < (int)g_[v].size(); ++i) {
          const auto& e = g_[v][i];
          if (e.cap > 0 && pv[e.to] == -1) {
            pv[e.to] = v;
            pe[e.to] = i;
            q.push(e.to);
          }
        }
      }
      if (pv[t] == -1) break;
      Cap add = (Cap)(limit - total);
      for (int v = t; v != s; v = pv[v]) {
        Cap c = g_[pv[v]][pe[v]].cap;
        if (c < add) add = c;
      }
      for (int v = t; v != s; v = pv[v]) {
        auto& e = g_[pv[v]][pe[v]];
        e.cap = (Cap)(e.cap - add);
        g_[v][e.rev].cap = (Cap)(g_[v][e.rev].cap + add);
      }
      total = (Cap)(total + add);
    }
    return total;
  }

  std::vector<bool> min_cut(int s) const {
    int n = (int)g_.size();
    std::vector<bool> vis(n, false);
    std::queue<int> q;
    vis[s] = true;
    q.push(s);
    while (!q.empty()) {
      int v = q.front();
      q.pop();
      for (const auto& e : g_[v]) {
        if (e.cap > 0 && !vis[e.to]) {
          vis[e.to] = true;
          q.push(e.to);
        }
      }
    }
    return vis;
  }

 private:
  struct RawEdge {
    int to;
    int rev;
    Cap cap;
  };
  std::vector<std::pair<int, int>> pos_;
  std::vector<std::vector<RawEdge>> g_;
};

template <typename Cap>
void assert_edge_eq(const typename MaxFlow<Cap>::Edge& lhs,
                    const typename MaxFlow<Cap>::Edge& rhs) {
  assert(lhs.from == rhs.from);
  assert(lhs.to == rhs.to);
  assert(lhs.cap == rhs.cap);
  assert(lhs.flow == rhs.flow);
}

template <typename Cap>
void verify_flow_and_cut(
    int n, int s, int t,
    const std::vector<std::pair<std::pair<int, int>, Cap>>& added,
    const MaxFlow<Cap>& mf, Cap total_flow, bool check_optimal_cut) {
  auto all_edges = mf.edges();
  int m = (int)added.size();
  assert((int)all_edges.size() == m);

  std::vector<Cap> balance(n, 0);
  for (int i = 0; i < m; ++i) {
    auto e = mf.get_edge(i);
    assert_edge_eq<Cap>(e, all_edges[i]);
    assert(e.from == added[i].first.first);
    assert(e.to == added[i].first.second);
    assert(e.cap == added[i].second);
    assert(e.flow >= 0 && e.flow <= e.cap);
    if (e.from != e.to) {
      balance[e.from] -= e.flow;
      balance[e.to] += e.flow;
    }
  }

  for (int v = 0; v < n; ++v) {
    if (v == s) {
      assert(balance[v] == -total_flow);
    } else if (v == t) {
      assert(balance[v] == total_flow);
    } else {
      assert(balance[v] == 0);
    }
  }

  if (check_optimal_cut) {
    auto cut = mf.min_cut(s);
    assert((int)cut.size() == n);
    assert(cut[s]);
    assert(!cut[t]);
    Cap cut_cap = 0;
    for (const auto& e : all_edges) {
      if (cut[e.from] && !cut[e.to]) {
        assert(e.flow == e.cap);
        cut_cap = (Cap)(cut_cap + e.cap);
      } else if (!cut[e.from] && cut[e.to]) {
        assert(e.flow == 0);
      }
    }
    assert(cut_cap == total_flow);
  }
}

void test_type_constraints() {
  static_assert(can_instantiate_max_flow<int>);
  static_assert(can_instantiate_max_flow<long long>);
  static_assert(can_instantiate_max_flow<__int128>);
  static_assert(can_instantiate_max_flow<unsigned __int128>);
  static_assert(!can_instantiate_max_flow<bool>);
  static_assert(!can_instantiate_max_flow<double>);
  static_assert(!can_instantiate_max_flow<const int>);
  static_assert(!can_instantiate_max_flow<volatile long long>);
}

template <typename Cap> void test_integer_type() {
  MaxFlow<Cap> mf(4);
  assert(mf.add_edge(0, 1, (Cap)3) == 0);
  assert(mf.add_edge(0, 2, (Cap)2) == 1);
  assert(mf.add_edge(1, 2, (Cap)1) == 2);
  assert(mf.add_edge(1, 3, (Cap)2) == 3);
  assert(mf.add_edge(2, 3, (Cap)3) == 4);

  assert(mf.flow(0, 3) == (Cap)5);
  auto cut = mf.min_cut(0);
  assert(cut[0] && !cut[3]);
  auto es = mf.edges();
  assert(es.size() == 5);
  for (int i = 0; i < 5; ++i) assert_edge_eq<Cap>(es[i], mf.get_edge(i));
  assert_edge_eq<Cap>(es[0], {0, 1, (Cap)3, (Cap)3});
  assert_edge_eq<Cap>(es[1], {0, 2, (Cap)2, (Cap)2});
}

void test_all_integer_types() {
  test_integer_type<char>();
  test_integer_type<signed char>();
  test_integer_type<unsigned char>();
  test_integer_type<wchar_t>();
  test_integer_type<char8_t>();
  test_integer_type<char16_t>();
  test_integer_type<char32_t>();
  test_integer_type<short>();
  test_integer_type<unsigned short>();
  test_integer_type<int>();
  test_integer_type<unsigned int>();
  test_integer_type<long>();
  test_integer_type<unsigned long>();
  test_integer_type<long long>();
  test_integer_type<unsigned long long>();
  test_integer_type<__int128>();
  test_integer_type<unsigned __int128>();
}

void test_empty_and_single() {
  MaxFlow<int> mf_default;
  assert(mf_default.edges().empty());

  MaxFlow<int> mf0(0);
  assert(mf0.edges().empty());

  MaxFlow<int> mf1(1);
  assert(mf1.add_edge(0, 0, 10) == 0);
  assert(mf1.add_edge(0, 0, 0) == 1);
  auto es = mf1.edges();
  assert(es.size() == 2);
  assert_edge_eq<int>(es[0], {0, 0, 10, 0});
  assert_edge_eq<int>(es[1], {0, 0, 0, 0});
  assert((mf1.min_cut(0) == std::vector<bool>{true}));
}

void test_copy_move_and_incremental() {
  MaxFlow<long long> mf(4);
  std::vector<std::pair<std::pair<int, int>, long long>> added = {
      {{0, 1}, 10}, {{1, 3}, 5}, {{0, 2}, 4}, {{2, 3}, 10}};
  for (int i = 0; i < (int)added.size(); ++i) {
    int id = mf.add_edge(added[i].first.first, added[i].first.second,
                         added[i].second);
    assert(id == i);
  }

  // Zero flow limit does not change the network.
  assert(mf.flow(0, 3, 0) == 0);
  verify_flow_and_cut(4, 0, 3, added, mf, 0LL, false);

  // Partial flow limit.
  assert(mf.flow(0, 3, 6) == 6);
  verify_flow_and_cut(4, 0, 3, added, mf, 6LL, false);

  // Copy before mutating further.
  MaxFlow<long long> copy_mf(mf);
  MaxFlow<long long> assign_mf;
  assign_mf = mf;
  MaxFlow<long long>& self_ref = assign_mf;
  assign_mf = self_ref;

  // Complete remaining flow on original.
  assert(mf.flow(0, 3) == 3);
  verify_flow_and_cut(4, 0, 3, added, mf, 9LL, true);
  assert(mf.flow(0, 3) == 0);

  // Copy still had only 6 units of flow.
  verify_flow_and_cut(4, 0, 3, added, copy_mf, 6LL, false);
  assert(copy_mf.flow(0, 3) == 3);
  verify_flow_and_cut(4, 0, 3, added, copy_mf, 9LL, true);

  // Add another edge after flow and augment more.
  mf.add_edge(1, 2, 5);
  added.push_back({{1, 2}, 5});
  assert(mf.flow(0, 3) == 5);
  verify_flow_and_cut(4, 0, 3, added, mf, 14LL, true);

  // Push flow in reverse direction (t -> s) cancels existing flow.
  assert(mf.flow(3, 0, 4) == 4);
  verify_flow_and_cut(4, 0, 3, added, mf, 10LL, false);

  // Move construction and assignment.
  MaxFlow<long long> moved_mf(std::move(mf));
  verify_flow_and_cut(4, 0, 3, added, moved_mf, 10LL, false);
  MaxFlow<long long> move_assign_mf;
  move_assign_mf = std::move(moved_mf);
  verify_flow_and_cut(4, 0, 3, added, move_assign_mf, 10LL, false);

  // Self-move and reuse of moved-from object.
  MaxFlow<long long>& move_self_ref = move_assign_mf;
  move_assign_mf = std::move(move_self_ref);
  move_assign_mf = assign_mf;
  assert(move_assign_mf.flow(0, 3) == 3);

  moved_mf = MaxFlow<long long>(2);
  moved_mf.add_edge(0, 1, 7);
  assert(moved_mf.flow(0, 1) == 7);
}

void test_special_and_boundary_cases() {
  // Disconnected s and t.
  {
    MaxFlow<int> mf(4);
    mf.add_edge(0, 1, 100);
    mf.add_edge(2, 3, 100);
    assert(mf.flow(0, 3) == 0);
    assert((mf.min_cut(0) == std::vector<bool>{true, true, false, false}));
  }

  // Self-loops, zero-capacity edges, and parallel edges.
  {
    MaxFlow<int> mf(3);
    std::vector<std::pair<std::pair<int, int>, int>> added = {
        {{0, 0}, 50}, {{0, 1}, 0}, {{0, 1}, 3},  {{0, 1}, 4},
        {{1, 1}, 20}, {{1, 2}, 5}, {{1, 2}, 10}, {{2, 0}, 7}};
    for (const auto& [uv, c] : added) mf.add_edge(uv.first, uv.second, c);
    int ans = mf.flow(0, 2);
    assert(ans == 7);
    verify_flow_and_cut(3, 0, 2, added, mf, ans, true);
    assert(mf.get_edge(0).flow == 0);
    assert(mf.get_edge(1).flow == 0);
    assert(mf.get_edge(4).flow == 0);
  }

  // Long path graph.
  {
    constexpr int n = 10000;
    MaxFlow<int> mf(n);
    for (int i = 0; i + 1 < n; ++i) mf.add_edge(i, i + 1, 1000000000);
    assert(mf.flow(0, n - 1) == 1000000000);
    auto cut = mf.min_cut(0);
    assert(cut[0] && !cut[n - 1]);
  }

  // Large capacities at INT_MAX / LLONG_MAX / ULLONG_MAX / __int128 limits.
  {
    MaxFlow<int> mf_i(3);
    mf_i.add_edge(0, 1, INT_MAX);
    mf_i.add_edge(1, 2, INT_MAX);
    assert(mf_i.flow(0, 2) == INT_MAX);

    MaxFlow<long long> mf_ll(3);
    mf_ll.add_edge(0, 1, LLONG_MAX);
    mf_ll.add_edge(1, 2, LLONG_MAX);
    assert(mf_ll.flow(0, 2) == LLONG_MAX);

    MaxFlow<unsigned long long> mf_ull(3);
    mf_ull.add_edge(0, 1, ULLONG_MAX);
    mf_ull.add_edge(1, 2, ULLONG_MAX);
    assert(mf_ull.flow(0, 2) == ULLONG_MAX);

    __int128 big = ((__int128)1 << 100) + 123456789;
    MaxFlow<__int128> mf_128(4);
    mf_128.add_edge(0, 1, big);
    mf_128.add_edge(1, 3, big);
    mf_128.add_edge(0, 2, big);
    mf_128.add_edge(2, 3, big);
    assert(mf_128.flow(0, 3) == 2 * big);

    unsigned __int128 ubig = ~(unsigned __int128)0;
    MaxFlow<unsigned __int128> mf_u128(3);
    mf_u128.add_edge(0, 1, ubig);
    mf_u128.add_edge(1, 2, ubig);
    assert(mf_u128.flow(0, 2) == ubig);
  }
}

void test_random_against_naive(std::mt19937_64& rng, int n, int m,
                               long long max_c) {
  MaxFlow<long long> mf(n);
  NaiveMaxFlow<long long> naive(n);
  std::vector<std::pair<std::pair<int, int>, long long>> added;
  added.reserve(m);

  std::uniform_int_distribution<int> vdist(0, n - 1);
  std::uniform_int_distribution<long long> cdist(0, max_c);
  for (int i = 0; i < m; ++i) {
    int u = vdist(rng);
    int v = vdist(rng);
    long long c = cdist(rng);
    assert(mf.add_edge(u, v, c) == naive.add_edge(u, v, c));
    added.push_back({{u, v}, c});
  }

  int s = vdist(rng);
  int t = vdist(rng);
  while (t == s) t = vdist(rng);

  // Test partial limit first, then full flow, then extra edges.
  long long limit1 = cdist(rng);
  long long f1 = mf.flow(s, t, limit1);
  long long nf1 = naive.flow(s, t, limit1);
  assert(f1 == nf1);
  verify_flow_and_cut(n, s, t, added, mf, f1, false);

  long long f2 = mf.flow(s, t);
  long long nf2 = naive.flow(s, t, (1LL << 62));
  assert(f2 == nf2);
  verify_flow_and_cut(n, s, t, added, mf, f1 + f2, true);

  for (int i = 0; i < 3; ++i) {
    int u = vdist(rng);
    int v = vdist(rng);
    long long c = cdist(rng);
    mf.add_edge(u, v, c);
    naive.add_edge(u, v, c);
    added.push_back({{u, v}, c});
  }
  long long f3 = mf.flow(s, t);
  long long nf3 = naive.flow(s, t, (1LL << 62));
  assert(f3 == nf3);
  verify_flow_and_cut(n, s, t, added, mf, f1 + f2 + f3, true);
}

void test_random_medium(std::mt19937_64& rng, int n, int m, long long max_c) {
  MaxFlow<long long> mf(n);
  std::vector<std::pair<std::pair<int, int>, long long>> added;
  added.reserve(m);
  std::uniform_int_distribution<int> vdist(0, n - 1);
  std::uniform_int_distribution<long long> cdist(0, max_c);
  for (int i = 0; i < m; ++i) {
    int u = vdist(rng);
    int v = vdist(rng);
    long long c = cdist(rng);
    mf.add_edge(u, v, c);
    added.push_back({{u, v}, c});
  }
  long long total = mf.flow(0, n - 1);
  verify_flow_and_cut(n, 0, n - 1, added, mf, total, true);
}

}  // namespace

int main() {
  test_type_constraints();
  test_all_integer_types();
  test_empty_and_single();
  test_copy_move_and_incremental();
  test_special_and_boundary_cases();

  std::mt19937_64 rng(0x3f84d5b5b5470917ULL);
  for (int n : {2, 4, 8, 20, 40}) {
    for (int m_mult : {0, 1, 3, 6}) {
      for (long long max_c : {1LL, 10LL, 1000000000LL}) {
        for (int trial = 0; trial < 3; ++trial) {
          test_random_against_naive(rng, n, n * m_mult, max_c);
        }
      }
    }
  }

  for (int n : {100, 300}) {
    for (int trial = 0; trial < 5; ++trial) {
      test_random_medium(rng, n, n * 5, 1000000000LL);
    }
  }
  return 0;
}
