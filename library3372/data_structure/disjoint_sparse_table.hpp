// Repository: Library3372
// File: library3372/data_structure/disjoint_sparse_table.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/data_structure/disjoint_sparse_table.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_DISJOINT_SPARSE_TABLE_HPP_
#define LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_DISJOINT_SPARSE_TABLE_HPP_

#include <algorithm>
#include <bit>
#include <cassert>
#include <limits>
#include <vector>

#include "library3372/internal/algebra/monoid.hpp"

namespace library3372 {

// Disjoint sparse table for static range monoid queries
// https://info.atcoder.jp/entry/algorithm_lectures/disjoint_sparse_table
// https://discuss.codechef.com/t/tutorial-disjoint-sparse-table/17404
template <typename S, auto op, auto e>
  requires internal::Monoid<S, op, e>
class DisjointSparseTable {
 public:
  DisjointSparseTable() = default;

  explicit DisjointSparseTable(const std::vector<S>& a) : n_((int)a.size()) {
    assert(a.size() <= (unsigned long long)std::numeric_limits<int>::max());
    if (n_ == 0) return;
    int h = (n_ == 1) ? 1 : std::bit_width((unsigned int)(n_ - 1));
    table_.reserve((unsigned long long)h * (unsigned long long)n_);
    table_.insert(table_.end(), a.begin(), a.end());
    if (h == 1) return;
    table_.resize((unsigned long long)h * (unsigned long long)n_,
                  Adapter::e_storage());
    for (int k = 1; k < h; ++k) {
      Storage* row =
          table_.data() + (unsigned long long)k * (unsigned long long)n_;
      long long half = 1LL << k;
      long long step = half << 1;
      for (long long mid = half; mid < n_; mid += step) {
        int m = (int)mid;
        row[m - 1] = table_[m - 1];
        for (int i = m - 2; i >= (int)(mid - half); --i)
          row[i] = Adapter::op_storage(table_[i], row[i + 1]);
        int end = (int)std::min(mid + half, (long long)n_);
        row[m] = table_[m];
        for (int i = m + 1; i < end; ++i)
          row[i] = Adapter::op_storage(row[i - 1], table_[i]);
      }
    }
  }

  S get(int p) const {
    assert(0 <= p && p < n_);
    return Adapter::to_s(table_[p]);
  }

  S prod(int l, int r) const {
    assert(0 <= l && l <= r && r <= n_);
    if (l == r) return Adapter::call_e();
    if (l + 1 == r) return Adapter::to_s(table_[l]);
    int k = std::bit_width((unsigned int)(l ^ (r - 1))) - 1;
    const Storage* row =
        table_.data() + (unsigned long long)k * (unsigned long long)n_;
    return Adapter::call_op(row[l], row[r - 1]);
  }

  S all_prod() const { return prod(0, n_); }

  int size() const { return n_; }

 private:
  using Adapter = internal::MonoidAdapter<S, op, e>;
  using Storage = typename Adapter::Storage;

  int n_ = 0;
  std::vector<Storage> table_;
};

}  // namespace library3372

#endif  // LIBRARY3372_LIBRARY3372_DATA_STRUCTURE_DISJOINT_SPARSE_TABLE_HPP_
