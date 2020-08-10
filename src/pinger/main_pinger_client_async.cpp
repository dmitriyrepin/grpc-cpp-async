#include <fmt/core.h>
#include <grpcpp/grpcpp.h>

#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "pinger_client_async.h"
#include "shared/aux.h"
#include "shared/queue_tag.h"

class CallbackReceiver {
 public:
  CallbackReceiver(int call_count, int call_msec)
      : call_msec_(call_msec), call_count_(call_count) {}

  void start() {
    call_processed_ = 0;
    start_ = std::chrono::high_resolution_clock::now();
  }

  void print_result(std::string result) {
    call_processed_++;
    fmt::print("PingerClient.ping result: {}\n", result);
    if (call_processed_ >= call_count_) {
      double msec = dgr::get_duration_msec(start_);
      fmt::print("---- Completion {} of {} msec-long requests took {} msec\n",  //
                 call_count_, call_msec_, msec);
    }
  }

 private:
  int call_msec_ = 3000;
  int call_count_ = 10;
  int call_processed_;
  std::chrono::system_clock::time_point start_;
};

int main(int argc, char **argv) {
  grpc::CompletionQueue cq;
  std::thread thread_ = std::thread(dgr::QueueTag::run_completion_queue_loop_forever, &cq);
  std::string host_port = "localhost:50062";
  dgr::PingerClientAsync client(host_port, &cq);
  int call_msec = 3000;
  int call_count = 10;
  CallbackReceiver receiver(call_count, call_msec);
  receiver.start();
  fmt::print("Running PingerClientAsync test @ {}\n", host_port);
  fmt::print("Press control-c to quit\n\n");
  for (int i = 0; i < call_count; i++) {
    std::string name = fmt::format("Caller_{}", i);
    client.ping(name, call_msec,
                std::bind(&CallbackReceiver::print_result, &receiver,  //
                          std::placeholders::_1));
  }

  thread_.join();  // blocks forever

  return 0;
}
