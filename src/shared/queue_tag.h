#pragma once

#include <fmt/core.h>
#include <grpcpp/grpcpp.h>

#include <functional>

namespace dgr
{

struct QueueTag
{
  std::function<void(void)> cb;
  std::string name;

  static void run_completion_queue_loop_forever(grpc::CompletionQueue *queue)
  {
    bool ok;
    void *tag;
    while (true)
    {
      GPR_ASSERT(queue->Next(&tag, &ok));
      GPR_ASSERT(ok);
      QueueTag *queue_tag = static_cast<QueueTag *>(tag);
      queue_tag->cb();
      delete queue_tag;
    }
  }
};

} // namespace dgr