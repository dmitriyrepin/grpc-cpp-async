#include <fmt/core.h>
#include <grpcpp/grpcpp.h>

#include <future>
#include <iostream>
#include <memory>
#include <string>

#include "talker.grpc.pb.h"

namespace dgr {

class TalkerClient {
 public:
  TalkerClient(std::string host_port)
      : stub_(Talker::NewStub(grpc::CreateChannel(host_port, grpc::InsecureChannelCredentials()))) {
  }

  std::string short_hello(const std::string& user) {
    ShortHelloRequest request;
    request.set_name(user);
    ShortHelloReply reply;
    grpc::ClientContext context;
    grpc::Status status = stub_->ShortHello(&context, request, &reply);
    if (status.ok()) {
      return reply.message();
    } else {
      std::string error =
          fmt::format("RPC failed: {} - {}", status.error_code(), status.error_message());
      fmt::print(stderr, "{}\n", error);
      return error;
    }
  }

  std::string long_bye(const std::string& user, int32_t delay_msec, bool use_async) {
    LongByeRequest request;
    request.set_name(user);
    request.set_ping_delay_msec(delay_msec);
    request.set_use_async_ping(use_async);
    LongByeReply reply;
    grpc::ClientContext context;
    grpc::Status status = stub_->LongBye(&context, request, &reply);
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
  std::unique_ptr<Talker::Stub> stub_;
};

}  // namespace dgr
