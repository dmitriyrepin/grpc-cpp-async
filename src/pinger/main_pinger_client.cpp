#include <string>

#include "pinger.grpc.pb.h"
#include "pinger_client.h"

int main(int argc, char **argv) {
  std::string host_port = "localhost:50062";
  fmt::print("Running PingerClient test @ {}\n", host_port);
  dgr::PingerClient pinger(host_port);
  for (int i = 0; i < 3; ++i) {
    std::string name = fmt::format("Caller_{}", i);
    std::string ping_reply = pinger.ping(name, (3 - i) * 1000);
    fmt::print("Ping reply: {}\n", ping_reply);
  }
  return 0;
}
