// Repository: Library3372
// File: library3372/internal/chrono/timer.hpp
// URL: https://github.com/prime3372/Library3372/blob/main/library3372/internal/chrono/timer.hpp
// Author: prime3372
// License: CC0 1.0 Universal

#ifndef LIBRARY3372_LIBRARY3372_INTERNAL_CHRONO_TIMER_HPP_
#define LIBRARY3372_LIBRARY3372_INTERNAL_CHRONO_TIMER_HPP_

#include <chrono>

namespace library3372::internal {

class Timer {
 private:
  using clock_t = std::chrono::high_resolution_clock;
  clock_t::time_point start_time_;

 public:
  Timer() : start_time_(clock_t::now()) {}

  void reset() { start_time_ = clock_t::now(); }

  double elapsed() const {
    auto end_time = clock_t::now();
    return std::chrono::duration<double>(end_time - start_time_).count();
  }
};

}  // namespace library3372::internal

#endif  // LIBRARY3372_LIBRARY3372_INTERNAL_CHRONO_TIMER_HPP_
