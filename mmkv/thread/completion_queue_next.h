#ifndef MMKVC_INTERNAL_THREAD_COMPELTION_QUEUE_NEXT_H__
#define MMKVC_INTERNAL_THREAD_COMPELTION_QUEUE_NEXT_H__

#include "completion_queue_interface.h"

namespace mmkv {

class CompletionQueueNext : public CompletionQueueInterface {
 public:
  CompletionQueueNext();
  ~CompletionQueueNext() noexcept;
  
  void Push(CompletionTask task) override;
  void Next(void **tag, bool *ok) override;
 private:
  std::deque<CompletionTask> cq_;
  kanon::Condition cq_cond_;
  kanon::MutexLock cq_mu_;
};

} // namespace mmkv

#endif // MMKVC_INTERNAL_THREAD_COMPELTION_QUEUE_NEXT_H__
