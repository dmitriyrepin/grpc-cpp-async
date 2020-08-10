#pragma once

#include <grpcpp/grpcpp.h>

#include <memory>
#include <string>

#include "pinger.grpc.pb.h"
#include "shared/queue_tag.h"

namespace dgr {

class PingerClientAsync {
 public:
  PingerClientAsync(const std::string &host_port, grpc::CompletionQueue *cq)
      : cq_ptr_(cq),
        context_(new grpc::ClientContext()),
        status_(),
        stub_(Pinger::NewStub(grpc::CreateChannel(host_port, grpc::InsecureChannelCredentials()))),
        request_(),
        reply_(),
        response_reader_(),
        result_cb_() {}

  void ping(const std::string &name, int32_t delay_msec,
            std::function<void(std::string)> result_cb) {
    result_cb_ = result_cb;

    context_.reset(new grpc::ClientContext());
    request_.set_name(name);
    request_.set_delay_msec(delay_msec);
    response_reader_ = stub_->PrepareAsyncPing(context_.get(), request_, cq_ptr_);
    response_reader_->StartCall();

    QueueTag *tag = new QueueTag();
    tag->cb = std::bind(&PingerClientAsync::ping_result, this);
    tag->name = "PingerClientAsync::ping_result";
    response_reader_->Finish(&reply_, &status_, (void *)tag);
  }

  void ping_result() {
    if (status_.ok()) {
      result_cb_(reply_.message());
    } else {
      std::string error =
          fmt::format("RPC failed: {} - {}", status_.error_code(), status_.error_message());
      fmt::print(stderr, "{}\n", error);
      result_cb_(error);
    }
  }

 protected:
  grpc::CompletionQueue *cq_ptr_;

  std::unique_ptr<grpc::ClientContext> context_;
  grpc::Status status_;
  std::unique_ptr<Pinger::Stub> stub_;

  PingRequest request_;
  PingReply reply_;
  std::unique_ptr<grpc::ClientAsyncResponseReader<PingReply>> response_reader_;
  std::function<void(std::string)> result_cb_;
};

}  // namespace dgr
