#include <fmt/core.h>
#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include "endpoint_handle-long_bye.h"
#include "endpoint_handle-short_hello.h"
#include "endpoint_queue_tag_processor.h"
#include "shared/queue_tag.h"
#include "talker.grpc.pb.h"
namespace dgr {

class ServerImpl final {
 public:
  ServerImpl(const std::string& host_port, const std::string& ping_host_port)
      : host_port_(host_port), ping_host_port_(ping_host_port) {}

  ~ServerImpl() {
    server_->Shutdown();
    cq_->Shutdown();
  }

  void Run() {
    grpc::ServerBuilder builder;
    builder.AddListeningPort(host_port_, grpc::InsecureServerCredentials());
    builder.RegisterService(&service_);
    cq_ = builder.AddCompletionQueue();
    server_ = builder.BuildAndStart();
    fmt::print("Server listening on {}\n", host_port_);
    handle_rpcs_in_completion_queue_loop();
  }

 private:
  void handle_rpcs_in_completion_queue_loop() {
    ShortHelloCreator creator_hello;
    EndpointQueueTagProcessor::add_new_instance_to_queue(&service_, cq_.get(), &creator_hello);
    LongByeCreator creator_bye(ping_host_port_, cq_.get());
    EndpointQueueTagProcessor::add_new_instance_to_queue(&service_, cq_.get(), &creator_bye);
    QueueTag::run_completion_queue_loop_forever(cq_.get());
  }

 private:
  std::string host_port_;
  std::string ping_host_port_;
  std::unique_ptr<grpc::ServerCompletionQueue> cq_;
  dgr::Talker::AsyncService service_;
  std::unique_ptr<grpc::Server> server_;
};

}  // namespace dgr

int main(int argc, char** argv) {
  dgr::ServerImpl server("0.0.0.0:50051", "0.0.0.0:50062");
  server.Run();

  return 0;
}
