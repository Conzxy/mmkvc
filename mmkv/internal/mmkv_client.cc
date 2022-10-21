#include "mmkv_client.h"

#include <kanon/net/connection/tcp_connection.h>
#include <kanon/net/event_loop_thread.h>
#include <kanon/util/ptr.h>

#include <utility>

using namespace mmkv;
using namespace mmkv::protocol;
using namespace kanon;

MmkvClient::MmkvClient(InetAddr const& serv_addr)
  : loop_thr_("MmkvClient")
  , cli_(NewTcpClient(loop_thr_.StartRun(), serv_addr, "MmkvClient"))
  , latch_(1)
  , codec_(MmbpResponse::GetPrototype())
{
  codec_.SetMessageCallback([this](TcpConnectionPtr const& conn, std::unique_ptr<MmbpMessage> msg, TimeStamp rt) {
    (void)rt;
    (void)conn;

    std::unique_ptr<MmbpResponse> response((MmbpResponse*)msg.release());
    SetValueFromResponse(values_.front(), *response);
    values_.pop_front();
    latch_.Countdown();
  });

  cli_->SetConnectionCallback([this](TcpConnectionPtr const& conn) {
    if (conn->IsConnected()) {
      codec_.SetUpConnection(conn);
      conn_ = conn.get();
      latch_.Countdown();
    } else {
      latch_.Countdown();
    }
  });

  cli_->Connect();
  latch_.Wait();
}

MmkvClient::MmkvClient(char const *ip, uint16_t port)
  : MmkvClient(InetAddr(ip, port))
{
}

MmkvClient::MmkvClient(StringView address)
  : MmkvClient(InetAddr(address))
{
}

MmkvClient::~MmkvClient() noexcept {
}

void MmkvClient::Close() {
  LatchIncr();
  cli_->Disconnect();
  latch_.Wait();
}

void MmkvClient::AddStr(String key, String value, bool *result) {
  MmbpRequest req{};
  req.command = STR_ADD;
  req.key = std::move(key); req.SetKey();
  req.value = std::move(value); req.SetValue();
  values_.emplace_back(MmkvValue {
    .value = result,
    .type = V_BOOL,
  });
  
  LatchIncr();
  codec_.Send(conn_, &req);
}

void MmkvClient::GetStr(String key, String &value) {
  MmbpRequest req;
  req.command = STR_GET;
  req.key = std::move(key);
  req.SetKey();
  values_.emplace_back(MmkvValue {
    .value = &value,
    .type = V_STR,
  });

  LatchIncr();
  codec_.Send(conn_, &req);
}
