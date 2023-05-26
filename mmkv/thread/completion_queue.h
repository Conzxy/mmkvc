#ifndef MMKV_INTERNAL_THREAD_COMPLETION_QUEUE_H__
#define MMKV_INTERNAL_THREAD_COMPLETION_QUEUE_H__

#include <memory>
#include "cq_common.h"

namespace mmkv {

class CompletionQueueInterface;

enum CompletionType {
  CQ_NEXT = 0,
  CQ_PLUCK,
};

class CompletionQueue {
 public:
  explicit CompletionQueue(CompletionType type=CQ_NEXT);

  ~CompletionQueue() noexcept;
  
  void Push(CompletionTask task);

  void Next(bool *ok);

  void Pluck(void *tag, bool *ok);

 private:
  std::unique_ptr<CompletionQueueInterface> cq_;
};

} // namespace mmkv

#endif // MMKV_INTERNAL_THREAD_COMPLETION_QUEUE_H__
