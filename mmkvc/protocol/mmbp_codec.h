#ifndef _MMKV_PROTOCOL_MMBP_CODEC_H_
#define _MMKV_PROTOCOL_MMBP_CODEC_H_

#include "kanon/net/connection/tcp_connection.h"
#include "kanon/util/noncopyable.h"

#include "mmbp_request.h"
#include "mmbp_response.h"
#include "mmbp.h"

namespace mmkv {
namespace protocol {

class MmbpCodec {
  DISABLE_EVIL_COPYABLE(MmbpCodec)

 public:
  enum ErrorCode : uint8_t {
    E_NOERROR = 0,
    E_INVALID_SIZE_HEADER,
    E_INVALID_CHECKSUM,
    E_INVALID_MESSAGE,
    E_NO_COMPLETE_MESSAGE, // This is not a error, just indicator
  };
 private:

  using MessageCallback = std::function<void(TcpConnectionPtr const&, std::unique_ptr<MmbpMessage>, TimeStamp)>;
  using ErrorCallback = std::function<void(TcpConnectionPtr const&, ErrorCode)>;

  using SizeHeaderType = uint32_t;
  using CheckSumType = uint32_t;

  static constexpr uint8_t SIZE_LENGTH = sizeof(SizeHeaderType);
  static constexpr uint8_t CHECKSUM_LENGTH = sizeof(CheckSumType);
  static constexpr uint32_t MAX_SIZE = 1 << 26; // 64MB
  static char const MMBP_TAG[];
  static uint8_t MMBP_TAG_SIZE;

 public:
  MmbpCodec(MmbpMessage* prototype);
  
  void SetUpConnection(TcpConnectionPtr const& conn);

  void SetMessageCallback(MessageCallback cb) {
    message_cb_ = std::move(cb);
  }
  
  void SetErrorCallback(ErrorCallback cb) {
    error_cb_ = std::move(cb);
  }

  void Send(TcpConnectionPtr const& conn, MmbpMessage const* message);
  void Send(TcpConnection* conn, MmbpMessage const* message);

  ErrorCode Parse(Buffer& buffer, MmbpMessage*& message);
  void SerializeTo(MmbpMessage const* message, OutputBuffer& buffer);

  static char const* GetErrorString(ErrorCode code) noexcept;

 private:
  void OnMessage(TcpConnectionPtr const& conn, Buffer& buffer, TimeStamp recv_time);

  static bool VerifyCheckSum(Buffer& buffer, SizeHeaderType size_header);
  static void ErrorHandle(TcpConnectionPtr const& conn, ErrorCode code);

  // Member data:
  MmbpMessage* prototype_;
  MessageCallback message_cb_;
  ErrorCallback error_cb_;
};

} // protocol
} // mmkv

#endif // _MMKV_PROTOCOL_MMBP_CODEC_H_
