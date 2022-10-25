#include "completion_queue_next.h"

#include <assert.h>

using namespace mmkv;
using namespace kanon;

CompletionQueueNext::CompletionQueueNext()
  : CompletionQueueInterface()
  , cq_cond_(cq_mu_)
{
}

CompletionQueueNext::~CompletionQueueNext() noexcept
{

}

void CompletionQueueNext::Push(CompletionTask task)
{
  MutexGuard g(cq_mu_);
  cq_.push_back(std::move(task));

  cq_cond_.Notify();
}

void CompletionQueueNext::Next(void **tag, bool *ok)
{
  MutexGuard g(cq_mu_);
  while (cq_.empty()) {
    cq_cond_.Wait();
  }
  
  assert(!cq_.empty());
  if (ok) {
    *ok = cq_.front().status == CS_OK;
  }
  if (tag) {
    *tag = cq_.front().tag;
  }

  cq_.pop_front();
}
