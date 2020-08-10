#pragma once

#include <fmt/core.h>
#include <grpcpp/grpcpp.h>

#include <memory>

#include "endpoint_handle.h"
#include "shared/queue_tag.h"
#include "talker.grpc.pb.h"

namespace dgr {

// class EndpointBase;  // endpoint_base.h includes call_processor.h

class EndpointQueueTagProcessor {
 public:
  static void add_new_instance_to_queue(dgr::Talker::AsyncService* service,  //
                                        grpc::ServerCompletionQueue* cq,     //
                                        EndpointHandleCreator* endpoint_creator) {
    EndpointQueueTagProcessor* call_processor =
        new EndpointQueueTagProcessor(service, cq, endpoint_creator);
    call_processor->on_create();
  }

  EndpointQueueTagProcessor(dgr::Talker::AsyncService* service,  //
                            grpc::ServerCompletionQueue* cq,     //
                            EndpointHandleCreator* endpoint_creator)
      : service_(service), cq_(cq), endpoint_(), endpoint_creator_(endpoint_creator) {
    endpoint_.reset(endpoint_creator_->create_instance(service, cq));
  }

  void on_create() {
    QueueTag* tag = new QueueTag();
    tag->cb = std::bind(&EndpointQueueTagProcessor::on_process, this);
    tag->name = "EndpointQueueTagProcessor::on_process";
    endpoint_->on_create(tag);
  }

  void on_process() {
    EndpointQueueTagProcessor::add_new_instance_to_queue(service_, cq_, endpoint_creator_);

    QueueTag* tag = new QueueTag();
    tag->cb = std::bind(&EndpointQueueTagProcessor::on_finish, this);
    tag->name = "EndpointQueueTagProcessor::on_finish";
    endpoint_->on_process(tag);
  }

  void on_finish() {
    delete this;
    // nobody else to call
  }

 private:
  dgr::Talker::AsyncService* service_;
  grpc::ServerCompletionQueue* cq_;
  std::unique_ptr<EndpointHandle> endpoint_;

  EndpointHandleCreator* endpoint_creator_;
};

}  // namespace dgr