#ifndef MMKVC_INTERNAL_THREAD_COMPLETION_QUEUE_PLUCK_H__
#define MMKVC_INTERNAL_THREAD_COMPLETION_QUEUE_PLUCK_H__

#include "completion_queue_interface.h"

#include <unordered_map>

namespace mmkv {

class CompletionQueuePluck : public CompletionQueueInterface {
  struct SyncContext {
    SyncContext()
      : mu_()
      , cond_(mu_)
    {}

    kanon::MutexLock mu_;
    kanon::Condition cond_;
  };
  
 public:
  CompletionQueuePluck();
  ~CompletionQueuePluck() noexcept;

  void Push(CompletionTask task) override;
  void Pluck(void *tag, bool *ok) override;

 private:
  SyncContext *GetSyncContext();

  kanon::ThreadLocal<SyncContext> tl_sync_ctx_;
  kanon::MutexLock sync_ctx_mu_;
   
  std::unordered_map<void*, CompletionTask> cm_;
  kanon::MutexLock cm_mu_;
};

} // namespace mmkv

#endif // MMKVC_INTERNAL_THREAD_COMPLETION_QUEUE_PLUCK_H__
