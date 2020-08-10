#include <grpcpp/grpcpp.h>

#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "fmt/core.h"
#include "pinger.grpc.pb.h"

class PingerServiceImpl final : public dgr::Pinger::Service {
  grpc::Status Ping(grpc::ServerContext* context,     //
                    const dgr::PingRequest* request,  //
                    dgr::PingReply* reply) override {
    int32_t msec = request->delay_msec();
    fmt::print("Received ping from: {} - {} msec\n", request->name(), msec);
    std::this_thread::sleep_for(std::chrono::milliseconds(msec));
    std::string reply_str = fmt::format("Ping back {} after {} msec", request->name(), msec);
    reply->set_message(reply_str);
    fmt::print("   {}\n", reply_str);
    return grpc::Status::OK;
  }
};

void RunServer() {
  std::string host_port("0.0.0.0:50062");
  grpc::ServerBuilder builder;
  builder.AddListeningPort(host_port, grpc::InsecureServerCredentials());
  PingerServiceImpl service;
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  fmt::print("Server listening on {}\n", host_port);
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
