#pragma once

#include <grpcpp/grpcpp.h>

#include "endpoint_handle.h"
#include "pinger/pinger_client.h"
#include "pinger/pinger_client_async.h"
#include "talker.grpc.pb.h"

namespace dgr {

class EndpointHandle_LongBye : public EndpointHandle {
 public:
  EndpointHandle_LongBye(dgr::Talker::AsyncService* service, grpc::ServerCompletionQueue* cq,  //
                         PingerClient* pinger_client, PingerClientAsync* pinger_client_async)
      : EndpointHandle(service, cq),
        request_(),
        reply_(),
        responder_(&ctx_),
        pinger_("localhost:50062"),
        pinger_async_("localhost:50062", cq) {}

  void on_create(QueueTag* tag) override {
    service_->RequestLongBye(&ctx_, &request_, &responder_, cq_, cq_, tag);
  }

  void on_process(QueueTag* tag) override {
    on_process_tag_ = tag;
    if (request_.use_async_ping()) {
      fmt::print("Processing LongBye from {} with ASYNC ping\n", request_.name());
      on_process_tag_ = tag;
      pinger_async_.ping(
          request_.name(), request_.ping_delay_msec(),
          std::bind(&EndpointHandle_LongBye::on_post_process, this, std::placeholders::_1));
    } else {
      fmt::print("Processing LongBye from {} with blocking ping\n", request_.name());
      on_post_process(pinger_.ping(request_.name(), request_.ping_delay_msec()));
    }
  }

  void on_post_process(std::string ping_result) {
    reply_.set_message("LongBye: " + ping_result);
    responder_.Finish(reply_, grpc::Status::OK, on_process_tag_);
  }

 private:
  QueueTag* on_process_tag_;

  dgr::LongByeRequest request_;
  dgr::LongByeReply reply_;
  grpc::ServerAsyncResponseWriter<dgr::LongByeReply> responder_;

  dgr::PingerClient pinger_;
  dgr::PingerClientAsync pinger_async_;
};

class LongByeCreator : public EndpointHandleCreator {
 public:
  LongByeCreator(const std::string& ping_host_port, grpc::ServerCompletionQueue* cq) {
    pinger_async_.reset(new PingerClientAsync(ping_host_port, cq));
    // pinger_.reset(new PingerClient(ping_host_port));
  }

  EndpointHandle* create_instance(dgr::Talker::AsyncService* service,  //
                                  grpc::ServerCompletionQueue* cq) override {
    return new EndpointHandle_LongBye(service, cq, pinger_.get(), pinger_async_.get());
  }

 private:
  std::unique_ptr<PingerClient> pinger_;
  std::unique_ptr<PingerClientAsync> pinger_async_;
};

}  // namespace dgr