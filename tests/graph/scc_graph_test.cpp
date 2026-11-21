// Repository: Library3372
// File: tests/graph/scc_graph_test.cpp
// URL: https://github.com/prime3372/Library3372/blob/main/tests/graph/scc_graph_test.cpp
// Author: prime3372
// License: CC0 1.0 Universal

#include "library3372/graph/scc_graph.hpp"

#include <algorithm>
#include <cassert>
#include <queue>
#include <random>
#include <utility>
#include <vector>

namespace {

using library3372::SccGraph;

void verify_ids_only(int n, const std::vector<std::pair<int, int>>& edges,
                     const SccGraph& g) {
  int k = g.count_groups();
  const auto& ids = g.ids();

  assert((int)ids.size() == n);
  assert(n == 0 ? k == 0 : k >= 1);

  for (int v = 0; v < n; ++v) {
    assert(0 <= ids[v] && ids[v] < k);
  }

  for (const auto& [u, v] : edges) {
    assert(ids[u] <= ids[v]);
  }

  // Check exact mutual reachability when n is small enough for O(V(V + E)).
  if (n <= 200) {
    std::vector<std::vector<int>> adj(n);
    for (const auto& [u, v] : edges) adj[u].push_back(v);

    std::vector<std::vector<char>> reach(n, std::vector<char>(n, 0));
    std::queue<int> q;
    for (int s = 0; s < n; ++s) {
      reach[s][s] = 1;
      q.push(s);
      while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int to : adj[u]) {
          if (!reach[s][to]) {
            reach[s][to] = 1;
            q.push(to);
          }
        }
      }
    }

    for (int u = 0; u < n; ++u) {
      for (int v = 0; v < n; ++v) {
        bool same_scc = reach[u][v] && reach[v][u];
        assert((ids[u] == ids[v]) == same_scc);
      }
    }
  }
}

void verify_scc(int n, const std::vector<std::pair<int, int>>& edges,
                const SccGraph& g) {
  verify_ids_only(n, edges, g);

  int k = g.count_groups();
  const auto& ids = g.ids();
  const auto& groups = g.groups();

  assert((int)groups.size() == k);
  std::vector<int> seen(n, 0);
  for (int id = 0; id < k; ++id) {
    const auto& comp = groups[id];
    assert(!comp.empty());
    for (int i = 0; i < (int)comp.size(); ++i) {
      int v = comp[i];
      assert(0 <= v && v < n);
      assert(ids[v] == id);
      assert(seen[v] == 0);
      seen[v] = 1;
      if (i > 0) assert(comp[i - 1] < comp[i]);
    }
  }
  for (int v = 0; v < n; ++v) assert(seen[v] == 1);
}

void test_empty_and_single() {
  SccGraph g_default;
  g_default.build_ids();
  verify_ids_only(0, {}, g_default);
  g_default.build();
  verify_scc(0, {}, g_default);
  assert(g_default.count_groups() == 0);
  assert(g_default.ids().empty());
  assert(g_default.groups().empty());

  SccGraph g0(0);
  g0.build();
  assert(g0.count_groups() == 0);
  assert(g0.ids().empty());
  assert(g0.groups().empty());

  SccGraph g1(1);
  g1.build_ids();
  verify_ids_only(1, {}, g1);
  g1.build();
  verify_scc(1, {}, g1);
  assert(g1.count_groups() == 1);
  assert((g1.ids() == std::vector<int>{0}));
  assert((g1.groups() == std::vector<std::vector<int>>{{0}}));

  g1.add_edge(0, 0);
  g1.build();
  verify_scc(1, {{0, 0}}, g1);
  assert(g1.count_groups() == 1);
}

void test_incremental_and_copy_move() {
  SccGraph g(4);
  std::vector<std::pair<int, int>> edges = {{0, 1}, {2, 3}};
  for (const auto& [u, v] : edges) g.add_edge(u, v);
  g.build_ids();
  verify_ids_only(4, edges, g);
  g.build();
  // Repeated build() / build_ids() calls are idempotent.
  g.build();
  g.build_ids();
  verify_scc(4, edges, g);
  assert(g.count_groups() == 4);

  // Copy before modifying further so original remains unchanged.
  SccGraph copy_g(g);
  SccGraph assign_g;
  assign_g = g;
  SccGraph& self_ref = assign_g;
  assign_g = self_ref;
  verify_scc(4, edges, copy_g);
  verify_scc(4, edges, assign_g);

  // Add edges to merge components into {0, 1} -> {2, 3}, then one big SCC.
  g.add_edge(1, 0);
  edges.emplace_back(1, 0);
  g.build_ids();
  verify_ids_only(4, edges, g);
  assert(g.count_groups() == 3);
  g.build();
  verify_scc(4, edges, g);
  verify_scc(4, {{0, 1}, {2, 3}}, copy_g);
  assert(copy_g.count_groups() == 4);

  g.add_edge(1, 2);
  g.add_edge(3, 2);
  edges.emplace_back(1, 2);
  edges.emplace_back(3, 2);
  g.build();
  verify_scc(4, edges, g);
  assert(g.count_groups() == 2);

  g.add_edge(3, 0);
  edges.emplace_back(3, 0);
  g.build();
  verify_scc(4, edges, g);
  assert(g.count_groups() == 1);

  SccGraph moved_g(std::move(g));
  verify_scc(4, edges, moved_g);
  SccGraph move_assign_g;
  move_assign_g = std::move(moved_g);
  verify_scc(4, edges, move_assign_g);

  // Self-move-assignment leaves the object in a valid state for reassignment.
  SccGraph& move_self_ref = move_assign_g;
  move_assign_g = std::move(move_self_ref);
  move_assign_g = copy_g;
  verify_scc(4, {{0, 1}, {2, 3}}, move_assign_g);

  // Moved-from object can also be safely reassigned and reused.
  moved_g = SccGraph(2);
  moved_g.add_edge(0, 1);
  moved_g.build();
  verify_scc(2, {{0, 1}}, moved_g);
}

void test_special_shapes() {
  // No edges
  {
    constexpr int n = 20;
    SccGraph g(n);
    g.build();
    verify_scc(n, {}, g);
    assert(g.count_groups() == n);
  }
  // Forward and reverse chains (also tests non-recursive DFS depth)
  {
    constexpr int n = 20000;
    SccGraph g(n);
    std::vector<std::pair<int, int>> edges;
    edges.reserve(n - 1);
    for (int i = 0; i + 1 < n; ++i) {
      g.add_edge(i, i + 1);
      edges.emplace_back(i, i + 1);
    }
    g.build();
    verify_scc(n, edges, g);
    assert(g.count_groups() == n);
    for (int i = 0; i < n; ++i) assert(g.ids()[i] == i);
  }
  {
    constexpr int n = 20000;
    SccGraph g(n);
    std::vector<std::pair<int, int>> edges;
    edges.reserve(n - 1);
    for (int i = n - 1; i > 0; --i) {
      g.add_edge(i, i - 1);
      edges.emplace_back(i, i - 1);
    }
    g.build();
    verify_scc(n, edges, g);
    assert(g.count_groups() == n);
    for (int i = 0; i < n; ++i) assert(g.ids()[i] == n - 1 - i);
  }
  // Single large cycle
  {
    constexpr int n = 20000;
    SccGraph g(n);
    std::vector<std::pair<int, int>> edges;
    edges.reserve(n);
    for (int i = 0; i < n; ++i) {
      int to = (i + 1) % n;
      g.add_edge(i, to);
      edges.emplace_back(i, to);
    }
    g.build();
    verify_scc(n, edges, g);
    assert(g.count_groups() == 1);
  }
  // Complete graph with self-loops and parallel edges
  {
    constexpr int n = 12;
    SccGraph g(n);
    std::vector<std::pair<int, int>> edges;
    for (int u = 0; u < n; ++u) {
      for (int v = 0; v < n; ++v) {
        g.add_edge(u, v);
        g.add_edge(u, v);
        edges.emplace_back(u, v);
        edges.emplace_back(u, v);
      }
    }
    g.build();
    verify_scc(n, edges, g);
    assert(g.count_groups() == 1);
  }
}

void test_random(std::mt19937_64& rng, int n, int m, bool ids_first) {
  SccGraph g(n);
  std::vector<std::pair<int, int>> edges;
  edges.reserve(m);
  std::uniform_int_distribution<int> vdist(0, n - 1);
  for (int i = 0; i < m; ++i) {
    int u = vdist(rng);
    int v = vdist(rng);
    g.add_edge(u, v);
    edges.emplace_back(u, v);
  }
  if (ids_first) {
    g.build_ids();
    verify_ids_only(n, edges, g);
  }
  g.build();
  verify_scc(n, edges, g);
}

}  // namespace

int main() {
  test_empty_and_single();
  test_incremental_and_copy_move();
  test_special_shapes();

  std::mt19937_64 rng(0x517cc1b727220a95ULL);
  for (int n : {1, 2, 5, 10, 30, 80, 150}) {
    for (int m_mult : {0, 1, 2, 4, 8}) {
      for (int trial = 0; trial < 4; ++trial) {
        test_random(rng, n, n * m_mult, (trial % 2) == 0);
      }
    }
  }
  return 0;
}
