#ifndef _MMKV_PROTOCOL_MMBP_REQUEST_
#define _MMKV_PROTOCOL_MMBP_REQUEST_

#include "mmbp.h"
#include "mmbp_util.h"
#include "command.h"

#include "mmkvc/util/macro.h"
#include "key_value.h"

namespace kanon {
  class AppendFile;
}

namespace mmkv {
namespace protocol {

class MmbpRequest;

namespace detail {

extern MmbpRequest prototype;

}

// 协议字段设置为public
// 避免丑陋的getter/setter
// e.g. 由于可能需要移动字段，因此需要提供writable getter
class MmbpRequest : public MmbpMessage {

 public:

  MmbpRequest();
  ~MmbpRequest() noexcept override;

  void SerializeTo(ChunkList& buffer) const override;
  void ParseFrom(Buffer& buffer) override;
  
  MmbpMessage *New() const override {
    return new MmbpRequest();
  }
  
  void SetKey() noexcept {
    SetBit(has_bits_[0], 0);
  }

  void SetValue() noexcept {
    SetBit(has_bits_[0], 1);
  }

  void SetExpireTime() noexcept {
    SetBit(has_bits_[0], 2);
  }

  void SetValues() noexcept {
    SetBit(has_bits_[0], 3);
  }

  void SetKvs() noexcept {
    SetBit(has_bits_[0], 4);
  }
  
  void SetCount() noexcept {
    SetBit(has_bits_[0], 5);
  }
  
  void SetRange() noexcept {
    SetBit(has_bits_[0], 6);
  }

  void SetVmembers() noexcept {
    SetBit(has_bits_[0], 7);
  }
  
  void SetWeightRange(double l, double r) noexcept {
    SetRange();
    range.left = MMKV_DOUBLE2INT(l);
    range.right = MMKV_DOUBLE2INT(r);
  }

  DRange GetWeightRange() noexcept {
    return { MMKV_INT2DOUBLE(range.left), MMKV_INT2DOUBLE(range.right) };
  }

  bool HasKey() const noexcept {
    return TestBit(has_bits_[0], 0);
  }

  bool HasValue() const noexcept {
    return TestBit(has_bits_[0], 1);
  }

  bool HasExpireTime() const noexcept {
    return TestBit(has_bits_[0], 2);
  }
  
  bool HasValues() const noexcept {
    return TestBit(has_bits_[0], 3);
  }
  
  bool HasKvs() const noexcept {
    return TestBit(has_bits_[0], 4);
  }
  
  bool HasCount() const noexcept {
    return TestBit(has_bits_[0], 5);
  }

  bool HasRange() const noexcept {
    return TestBit(has_bits_[0], 6);
  }
  
  bool HasVmembers() const noexcept {
    return TestBit(has_bits_[0], 7);
  }

  bool HasNone() const noexcept {
    return has_bits_[0] == 0;
  }
  
  void DebugPrint() const noexcept;

  // void LogToFile(kanon::AppendFile& file) const;

  static MmbpRequest* GetPrototype() {
    return &detail::prototype;
  }
 
 private: 
  uint8_t has_bits_[1];
 public: 
  uint16_t command; // required


  String key; // optional
  
  // optional 
  // algo::ReservedArray<String> keys_; // for mget, etc.
  StrKvs kvs; // for madd, etc.
  StrValues values; // for lappend, lprepend, sadd, mget(reuse), etc.
  String value; // for stradd, strset, etc.
  WeightValues vmembers;  

  uint64_t expire_time; // optional

  union {
    uint32_t count;
    Range range;
  };

};

} // protocol
} // mmkv

#endif // _MMKV_PROTOCOL_MMBP_REQUEST_
