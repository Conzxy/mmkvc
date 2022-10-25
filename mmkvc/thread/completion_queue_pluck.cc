#include "completion_queue_pluck.h"

using namespace mmkv;
using namespace kanon;

CompletionQueuePluck::CompletionQueuePluck()
{

}

CompletionQueuePluck::~CompletionQueuePluck() noexcept
{

}

void CompletionQueuePluck::Push(CompletionTask task)
{
  auto ctx = GetSyncContext();
  {
  MutexGuard g(cm_mu_);
  cm_.insert({task.tag, std::move(task)});
  }
  ctx->cond_.Notify();
}

void CompletionQueuePluck::Pluck(void *tag, bool *ok)
{
  auto ctx = GetSyncContext();
  bool cond = false;

  MutexGuard g(ctx->mu_);
  while (true) {
    {
    MutexGuard cm_g(cm_mu_);
    cond = cm_.empty();
    }

    if (cond) {
      ctx->cond_.Wait();
    }
  }

  auto iter = cm_.find(tag);
  
  if (ok) {
    *ok = (iter != cm_.end()) && (iter->second.status == CS_OK);
  }
  if (iter != cm_.end()) cm_.erase(iter);
}

inline auto CompletionQueuePluck::GetSyncContext() -> SyncContext*
{
  SyncContext *ctx = nullptr;
  {
    MutexGuard g(sync_ctx_mu_);
    ctx = &tl_sync_ctx_.value();
  }
  
  return ctx;
}
