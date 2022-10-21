#ifndef _MMKV_CLIENT_MMKV_CLIENT_H_
#define _MMKV_CLIENT_MMKV_CLIENT_H_

#include <kanon/util/noncopyable.h>
#include <kanon/net/user_client.h>
#include <kanon/thread/count_down_latch.h>
#include <kanon/net/event_loop_thread.h>

#include "mmbp_codec.h"
#include "mmbp_response.h"
#include "mmkv_value.h"

#include <deque>

namespace mmkv {

using kanon::CountDownLatch;
using mmkv::protocol::MmbpCodec;
using mmkv::protocol::StatusCode;
using mmkv::algo::String;

class MmkvClient : kanon::noncopyable {
 public:
  explicit MmkvClient(InetAddr const& serv_addr);
  MmkvClient(char const *ip, uint16_t port);
  explicit MmkvClient(StringView address);

  ~MmkvClient() noexcept; 

  /*-----------------------------------------*/
  /* String API                              */
  /*-----------------------------------------*/

  void AddStr(String key, String value, bool *result);
  void GetStr(String key, String &value);

  StatusCode status_code() const noexcept { return status_code_; }
  char const *GetStatusMessage() const noexcept { return protocol::GetStatusMessage(status_code_); }

  void Wait() { latch_.Wait(); }
  void Close();

 private:
  void LatchIncr() const noexcept { latch_.Reset(latch_.GetCount()+1); }
  
  EventLoopThread loop_thr_;
  TcpClientPtr cli_; 
  mutable  CountDownLatch latch_;
  TcpConnection* conn_;
  MmbpCodec codec_;

  StatusCode status_code_;
  std::deque<MmkvValue> values_;
};

} // mmkv

#endif // _MMKV_CLIENT_MMKV_CLIENT_H_
