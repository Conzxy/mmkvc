#ifndef MMKVC_INTERNAL_MMKV_VALUE_H__
#define MMKVC_INTERNAL_MMKV_VALUE_H__

#include "mmkvc/protocol/mmbp_response.h"
#include "mmkvc/protocol/type.h"

namespace mmkv {

using protocol::MmbpResponse;
using protocol::StatusCode;
using protocol::String;
using protocol::StrValues;
using protocol::StrKvs;
using protocol::WeightValues;
using protocol::Weight;

struct MmkvValue {
  class MmkvClient;
 public:
  /**
   * Allow default construct only
   */
  MmkvValue()
    : type_(V_INVALID)
  {}

  ~MmkvValue() noexcept;
  
  MmkvValue(MmkvValue const &rhs);
  MmkvValue(MmkvValue &&rhs) noexcept;

  MmkvValue &operator=(MmkvValue const &rhs);
  MmkvValue &operator=(MmkvValue &&rhs) noexcept;
  
  void swap(MmkvValue &rhs) noexcept;

  String &ToStrValue() noexcept
  {
    ASSERT(type_ == V_STR_VALUE);
    return value;
  }

  StrValues &ToStrValues() noexcept
  {
    ASSERT(type_ == V_STR_VALUES);
    return values;
  }

  uint64_t ToCount() noexcept
  {
    ASSERT(type_ == V_COUNT);
    return count;
  }

  double ToWeight() noexcept
  {
    ASSERT(type_ == V_WEIGHT);
    return weight;
  }

  StatusCode status_code() noexcept
  {
    return status_code_;
  }

  StrKvs &ToStrKvs() noexcept
  {
    ASSERT(type_ == V_STR_VALUES);
    return kvs;
  }

  WeightValues &ToWeightValues() noexcept
  {
    ASSERT(type_ == V_WEIGHT_VALUES);
    return wvs;
  }

  void FromResponse(MmbpResponse &r);

 private:
  enum ValueType : int {
    V_INVALID = 0,
    V_COUNT,
    V_STR_VALUE,
    V_STR_VALUES,
    V_STR_KVS,
    V_WEIGHT_VALUES,
    V_WEIGHT,
    V_USR,
  };

  StatusCode status_code_;
  ValueType type_;

  union {
    uint64_t count;
    String value;
    StrValues values;
    StrKvs kvs;
    WeightValues wvs;
    double weight;
    void *data;
  };

};

} // mmkv

#endif // MMKVC_INTERNAL_MMKV_VALUE_H__
