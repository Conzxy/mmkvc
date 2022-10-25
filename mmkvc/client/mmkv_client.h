#ifndef _MMKV_CLIENT_MMKV_CLIENT_H_
#define _MMKV_CLIENT_MMKV_CLIENT_H_

#include <kanon/util/noncopyable.h>
#include <kanon/net/user_client.h>
#include <kanon/thread/count_down_latch.h>
#include <kanon/net/event_loop_thread.h>

#include "mmkvc/protocol/mmbp_codec.h"
#include "mmkvc/protocol/mmbp_response.h"
#include "mmkvc/protocol/mmbp_request.h"
#include "mmkv_value.h"

#include <deque>

namespace mmkv {

using mmkv::protocol::MmbpCodec;
using mmkv::protocol::MmbpRequest;
using mmkv::algo::String;

class MmkvClient : kanon::noncopyable {
  using Callback = std::function<void(MmkvValue &)>;

 public:
  explicit MmkvClient(InetAddr const& serv_addr);
  explicit MmkvClient(StringView addr)
    : MmkvClient(InetAddr(addr))
  {
  }

  ~MmkvClient() noexcept; 

  /*-----------------------------------------*/
  /* String API                              */
  /*-----------------------------------------*/

  void AddStr(String key, String value, Callback cb);
  void GetStr(String key, Callback cb);

  // StatusCode status_code() const noexcept { return status_code_; }
  // char const *GetStatusMessage() const noexcept { return protocol::GetStatusMessage(status_code_); }
  void Close();
  
 private:
  void PrepareOutstandingCall(MmbpRequest &req, Callback &cb); 

  EventLoopThread loop_thr_;
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

} // mmkv

#endif // _MMKV_CLIENT_MMKV_CLIENT_H_
