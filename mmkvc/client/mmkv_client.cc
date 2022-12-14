#include "mmkv_client.h"

#include <kanon/net/connection/tcp_connection.h>
#include <kanon/net/event_loop_thread.h>
#include <kanon/util/ptr.h>

#include <utility>

using namespace mmkv;
using namespace mmkv::protocol;
using namespace kanon;

MmkvClient::MmkvClient(InetAddr const &serv_addr)
  : loop_thr_("MmkvClient")
  , cli_(NewTcpClient(loop_thr_.StartRun(), serv_addr, "MmkvClient"))
  , codec_(MmbpResponse::GetPrototype())
{
  codec_.SetMessageCallback([this](TcpConnectionPtr const &conn,
                                   std::unique_ptr<MmbpMessage> msg,
                                   TimeStamp) {
    assert(pending_requestq_.empty());
    std::unique_ptr<MmbpResponse> response((MmbpResponse *)msg.release());

    /* Consume the callback and value */
    auto &outstanding_call = cbq_.front();
    outstanding_call.value.FromResponse(*response);
    outstanding_call.cb(outstanding_call.value);

    cbq_.pop_front();
  });

  cli_->SetConnectionCallback([this](TcpConnectionPtr const &conn) {
    if (conn->IsConnected()) {
      codec_.SetUpConnection(conn);

      /** Consume all pending request before connection established */
      MutexGuard g(pending_requestq_mu_);
      for (auto const &req : pending_requestq_)
        codec_.Send(conn, &req);
      pending_requestq_.clear();
    }
  });

  cli_->Connect();
}

MmkvClient::~MmkvClient() noexcept { LOG_TRACE << "MmkvClient is destroyed"; }

void MmkvClient::Close()
{
  LOG_WARN << "Close connection";
  cli_->Disconnect();
}

void MmkvClient::AddStr(String key, String value, Callback cb)
{
  MmbpRequest req;
  req.command = STR_ADD;
  req.key = std::move(key);
  req.SetKey();
  req.value = std::move(value);
  req.SetValue();

  PrepareOutstandingCall(req, cb);
}

void MmkvClient::GetStr(String key, Callback cb)
{
  MmbpRequest req;
  req.command = STR_GET;
  req.key = std::move(key);
  req.SetKey();

  PrepareOutstandingCall(req, cb);
}

std::future<bool> MmkvClient::AddStr(String key, String value) 
{
  std::promise<bool> ret;
  auto f= ret.get_future();
  AddStr(std::move(key), std::move(value), [p = std::move(ret)](MmkvValue &v) mutable {
    p.set_value(v.status_code() == protocol::S_OK);
  });

  return f;
}

std::future<String> MmkvClient::GetStr(String key)
{
  std::promise<String> ret;
  auto f = ret.get_future();
  GetStr(std::move(key), [p = std::move(ret)](MmkvValue &v) mutable {
    p.set_value(v.ToStrValue());
  });
  
  return f;
}

void MmkvClient::PrepareOutstandingCall(MmbpRequest &req, Callback &cb)
{
  MmkvValue value;
  {
    MutexGuard g(cbq_mu_);
    cbq_.push_back(OutstandingCall{std::move(value), std::move(cb)});
  }

  TcpConnectionPtr conn;
  if (!(conn = cli_->GetConnection())) {
    MutexGuard g(pending_requestq_mu_);
    pending_requestq_.push_back(std::move(req));
  } else {
    codec_.Send(conn, &req);
  }
}
