#ifndef MMKV_INTERNAL_THREAD_CQ_COMMON_H_
#define MMKV_INTERNAL_THREAD_CQ_COMMON_H_

enum CompletionStatus {
  CS_OK = 0,
  CS_FAILURE,
};

struct CompletionTask {
  void *tag;
  CompletionStatus status;
};
  
#endif
