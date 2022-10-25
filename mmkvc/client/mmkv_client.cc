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
  , codec_(MmbpResponse::GetPrototype())
{
  codec_.SetMessageCallback([this](TcpConnectionPtr const& conn, std::unique_ptr<MmbpMessage> msg, TimeStamp) {
    assert(pending_requestq_.empty());
    std::unique_ptr<MmbpResponse> response((MmbpResponse*)msg.release());
    auto &outstanding_call = cbq_.front();
    SetValueFromResponse(outstanding_call.value, *response);
    outstanding_call.cb(outstanding_call.value);
    cbq_.pop_front();
  });

  cli_->SetConnectionCallback([this](TcpConnectionPtr const& conn) {
    if (conn->IsConnected()) {
      codec_.SetUpConnection(conn);

      MutexGuard g(pending_requestq_mu_);
      for (auto const &req : pending_requestq_)
        codec_.Send(conn, &req);
      pending_requestq_.clear();
    }
  });

  cli_->Connect();
}

MmkvClient::~MmkvClient() noexcept {
  LOG_TRACE << "MmkvClient is destroyed";
}

void MmkvClient::Close() {
  cli_->Disconnect();
}

void MmkvClient::AddStr(String key, String value, Callback cb) {
  MmbpRequest req;
  req.command = STR_ADD;
  req.key = std::move(key); req.SetKey();
  req.value = std::move(value); req.SetValue();

  PrepareOutstandingCall(req, cb);
}

void MmkvClient::GetStr(String key, Callback cb) {
  MmbpRequest req;
  req.command = STR_GET;
  req.key = std::move(key);
  req.SetKey();
  
  PrepareOutstandingCall(req, cb);
}

void MmkvClient::PrepareOutstandingCall(MmbpRequest &req, Callback &cb)
{
  MmkvValue value;
  {
    MutexGuard g(cbq_mu_);
    cbq_.push_back(OutstandingCall{ std::move(value), std::move(cb) });
  }
  
  TcpConnectionPtr conn;
  if (!(conn = cli_->GetConnection())) {
    MutexGuard g(pending_requestq_mu_);
    pending_requestq_.push_back(std::move(req)); 
  } else {
    codec_.Send(conn, &req);
  }
}
