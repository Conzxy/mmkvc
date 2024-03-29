#ifndef _MMKV_CLIENT_MMKV_CLIENT_H_
#define _MMKV_CLIENT_MMKV_CLIENT_H_

#include <deque>
#include <future>

#include <kanon/net/event_loop_thread.h>
#include <kanon/net/user_client.h>
#include <kanon/thread/count_down_latch.h>
#include <kanon/util/noncopyable.h>

#include "mmkv/protocol/mmbp_codec.h"
#include "mmkv/protocol/mmbp_request.h"
#include "mmkv/protocol/mmbp_response.h"
#include "mmkv/util/move_only_function.h"
#include "mmkv_value.h"

namespace mmkv {

using mmkv::algo::String;
using mmkv::protocol::MmbpCodec;
using mmkv::protocol::MmbpRequest;

class MmkvClient : kanon::noncopyable {
  // using Callback = std::function<void(MmkvValue &)>;
  using Callback = zstl::MoveOnlyFunction<void(MmkvValue &)>;

 public:
  MmkvClient(EventLoop *loop, StringView addr);

  ~MmkvClient() noexcept;

  /*-----------------------------------------*/
  /* String API                              */
  /*-----------------------------------------*/

  void AddStr(String key, String value, Callback cb);
  void GetStr(String key, Callback cb);

  /*--------------------------------------------------*/
  /* Promise+Future version                           */
  /*--------------------------------------------------*/

  std::future<bool> AddStr(String key, String value);
  std::future<String> GetStr(String key);

  void Close();

 private:
  void PrepareOutstandingCall(MmbpRequest &req, Callback &cb);

  TcpClientPtr cli_;
  MmbpCodec codec_;

  std::deque<protocol::MmbpRequest> pending_requestq_;
  kanon::MutexLock pending_requestq_mu_;

  struct OutstandingCall {
    MmkvValue value;
    Callback cb;
  };

  std::deque<OutstandingCall> cbq_;
  kanon::MutexLock cbq_mu_;
};

} // namespace mmkv

#endif // _MMKV_CLIENT_MMKV_CLIENT_H_
