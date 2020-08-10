#pragma once

#include <fmt/core.h>

#include <chrono>

namespace dgr {

double get_duration_msec(std::chrono::time_point<std::chrono::high_resolution_clock> start) {
  auto stop = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> time_sec = stop - start;
  double time_msec = 1000.0 * time_sec.count();
  return time_msec;
}

template <typename R>
bool is_ready(std::future<R> const& f) {
  return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

void process_futures(std::vector<std::future<std::string>>& futures, int delay_msec) {
  int count = futures.size();
  fmt::print("Processing {} futures\n", futures.size());
  auto start = std::chrono::high_resolution_clock::now();
  while (futures.size() > 0) {
    std::vector<std::future<std::string>>::iterator it = futures.begin();
    while (it != futures.end()) {
      if (is_ready(*it)) {
        fmt::print("Replay - {}\n", (*it).get());
        it = futures.erase(it);
      } else {
        ++it;
      }
    }
  }
  double msec = get_duration_msec(start);
  fmt::print("---- Completing {} {}-msec-long requests took {} msec\n", count, delay_msec, msec);
}

}  // namespace dgr