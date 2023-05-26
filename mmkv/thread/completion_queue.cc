#include "completion_queue.h"

#include "completion_queue_next.h"
#include "completion_queue_pluck.h"

using namespace mmkv;

CompletionQueue::CompletionQueue(CompletionType type)
{
  switch (type) {
    case CQ_NEXT:
      cq_.reset(new CompletionQueueNext());
      break;
    case CQ_PLUCK:
      cq_.reset(new CompletionQueuePluck());
      break;
  }
}

CompletionQueue::~CompletionQueue() noexcept
{

}

void CompletionQueue::Push(CompletionTask task)
{
  cq_->Push(std::move(task));
}

void CompletionQueue::Next(bool *ok)
{
  cq_->Next(ok);
}

void CompletionQueue::Pluck(void *tag, bool *ok)
{
  cq_->Pluck(tag, ok);
}
