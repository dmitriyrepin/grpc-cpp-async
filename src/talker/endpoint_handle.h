#pragma once

#include <fmt/core.h>
#include <grpcpp/grpcpp.h>

#include <memory>

#include "shared/queue_tag.h"
#include "talker.grpc.pb.h"

namespace dgr {

// class EndpointQueueTagProcessor;  // call_processor.h includes endpoint_base.h

class EndpointHandle {
 public:
  EndpointHandle(dgr::Talker::AsyncService* service, grpc::ServerCompletionQueue* cq)
      : service_(service), cq_(cq), ctx_() {}

  virtual void on_create(QueueTag* tag) = 0;
  virtual void on_process(QueueTag* tag) = 0;

 protected:
  dgr::Talker::AsyncService* service_;
  grpc::ServerCompletionQueue* cq_;
  grpc::ServerContext ctx_;
};

class EndpointHandleCreator {
 public:
  virtual EndpointHandle* create_instance(dgr::Talker::AsyncService* service,  //
                                          grpc::ServerCompletionQueue* cq) = 0;
};

}  // namespace dgr