#include "mmkv_client.h"

#include <kanon/net/connection/tcp_connection.h>
#include <kanon/net/event_loop_thread.h>
#include <kanon/util/ptr.h>

#include <utility>

using namespace mmkv;
using namespace mmkv::protocol;
using namespace kanon;

MmkvClient::MmkvClient(EventLoop *p_loop, StringView addr)
  : cli_(NewTcpClient(p_loop, InetAddr(addr), "MmkvClient"))
  , codec_(MmbpResponse::GetPrototype())
{
  codec_.SetMessageCallback([this](TcpConnectionPtr const &conn, Buffer &buffer,
                                   size_t payload_size, TimeStamp) {
    // assert(pending_requestq_.empty());

    MmbpResponse resp;
    resp.ParseFrom(buffer);
    /* Consume the callback and value */

    OutstandingCall outstanding_call;
    {
      MutexGuard guard(cbq_mu_);
      outstanding_call = std::move(cbq_.front());
      cbq_.pop_front();
    }

    outstanding_call.value.FromResponse(resp);
    if (outstanding_call.cb) outstanding_call.cb(outstanding_call.value);
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
  auto f = ret.get_future();
  AddStr(std::move(key), std::move(value),
         [p = std::move(ret)](MmkvValue &v) mutable {
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
