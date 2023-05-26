#ifndef MMKVC_INTERNAL_THREAD_COMPLETION_QUEUE_INTERFACE_H__
#define MMKVC_INTERNAL_THREAD_COMPLETION_QUEUE_INTERFACE_H__

#include <deque>

#include "kanon/util/check.h"
#include "kanon/thread/thread_local.h"
#include "kanon/thread/condition.h"
#include "cq_common.h"

namespace mmkv {

class CompletionQueueInterface {
 public:
  explicit CompletionQueueInterface() = default;
  
  /* Note: The virtual isn't neccessary */
  virtual ~CompletionQueueInterface() noexcept = default;

  virtual void Push(CompletionTask task) = 0;

  virtual void Next(void **tag, bool *ok)
  {
    ASSERT(false && "Next() can't be called");
  }
  virtual void Pluck(void *tag, bool *ok)
  {
    ASSERT(false && "Pluck() can't be called");
  }
};

} // namespace mmkv

#endif // MMKVC_INTERNAL_THREAD_COMPLETION_QUEUE_INTERFACE_H__
