#pragma once

#include <grpcpp/grpcpp.h>

#include "endpoint_handle.h"
#include "shared/queue_tag.h"
#include "talker.grpc.pb.h"

namespace dgr {

class EndpointHandle_ShortHello : public EndpointHandle {
 public:
  EndpointHandle_ShortHello(dgr::Talker::AsyncService* service,  //
                            grpc::ServerCompletionQueue* cq)
      : EndpointHandle(service, cq), request_(), reply_(), responder_(&ctx_) {  //
  }

  void on_create(QueueTag* tag) override {
    service_->RequestShortHello(&ctx_, &request_, &responder_, cq_, cq_, tag);
  }

  void on_process(QueueTag* tag) override {
    fmt::print("ShortHello request from {}\n", request_.name());
    reply_.set_message("Hello " + request_.name());
    responder_.Finish(reply_, grpc::Status::OK, tag);
  }

 private:
  dgr::ShortHelloRequest request_;
  dgr::ShortHelloReply reply_;
  grpc::ServerAsyncResponseWriter<dgr::ShortHelloReply> responder_;
};

class ShortHelloCreator : public EndpointHandleCreator {
 public:
  EndpointHandle* create_instance(dgr::Talker::AsyncService* service,  //
                                  grpc::ServerCompletionQueue* cq) override {
    return new EndpointHandle_ShortHello(service, cq);
  }
};

}  // namespace dgr