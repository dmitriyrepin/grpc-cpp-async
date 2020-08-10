#include <fmt/core.h>
#include <grpcpp/grpcpp.h>

#include <future>
#include <iostream>
#include <memory>
#include <string>

#include "shared/aux.h"
#include "talker.grpc.pb.h"
#include "talker_client.h"

int main(int argc, char** argv) {
  std::string host_port = "localhost:50051";
  dgr::TalkerClient client(host_port);

  size_t num_requests = 10;

  fmt::print("Calling short hello endpoint\n");
  std::vector<std::future<std::string>> hello_futures;
  for (size_t i = 0; i < num_requests; ++i) {
    hello_futures.push_back(  //
        std::async(
            std::launch::async,                                          //
            [&client](std::string s) { return client.short_hello(s); },  //
            fmt::format("ShortHelloClinet_{}", i))                       //
    );
  }
  dgr::process_futures(hello_futures, 0);

  int32_t ping_delay_msec = 1000;
  for (bool use_async : {false, true}) {
    // for (bool use_async : {true}) {
    fmt::print("\nCalling long bye endpoint with {} ping delay (using async: {})\n",
               ping_delay_msec, use_async);
    std::vector<std::future<std::string>> bye_futures;
    for (size_t i = 0; i < num_requests; ++i) {
      bye_futures.push_back(  //
          std::async(
              std::launch::async,                                       //
              [&client, &ping_delay_msec, &use_async](std::string s) {  //
                return client.long_bye(s, ping_delay_msec, use_async);  //
              },                                                        //
              fmt::format("LongByeClinet_{}", i))                       //
      );
    }
    dgr::process_futures(bye_futures, ping_delay_msec);
  }
}
