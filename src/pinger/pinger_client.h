

#include <fmt/core.h>
#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>

#include "pinger.grpc.pb.h"

namespace dgr {

class PingerClient {
 public:
  PingerClient(std::string host_port)
      : stub_(Pinger::NewStub(grpc::CreateChannel(host_port, grpc::InsecureChannelCredentials()))) {
  }

  std::string ping(const std::string &user, int32_t msec) {
    PingRequest request;
    request.set_name(user);
    request.set_delay_msec(msec);
    PingReply reply;
    grpc::ClientContext context;
    grpc::Status status = stub_->Ping(&context, request, &reply);
    if (status.ok()) {
      return reply.message();
    } else {
      std::string error =
          fmt::format("RPC failed: {} - {}", status.error_code(), status.error_message());
      fmt::print(stderr, "{}\n", error);
      return error;
    }
  }

 private:
  std::unique_ptr<Pinger::Stub> stub_;
};

}  // namespace dgr
