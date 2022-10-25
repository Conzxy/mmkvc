#include "mmkv_value.h"

using namespace mmkv::protocol;

namespace mmkv {

MmkvValue::~MmkvValue() noexcept
{
  switch (type_) {
    case V_STR_VALUE:
      value.~String();
      break;
    case V_STR_VALUES:
      values.~StrValues();
      break;
    case V_STR_KVS:
      kvs.~StrKvs();
      break;
    case V_WEIGHT_VALUES:
      wvs.~WeightValues();
      break;
  }

  type_ = V_INVALID;
}

MmkvValue::MmkvValue(MmkvValue const &rhs)
  : status_code_(rhs.status_code_)
  , type_(rhs.type_)
{
  switch (rhs.type_) {
    case V_COUNT:
      count = rhs.count;
      break;
    case V_STR_VALUE:
      value = rhs.value;
      break;
    case V_STR_VALUES:
      values = rhs.values;
      break;
    case V_STR_KVS:
      kvs = rhs.kvs;
      break;
    case V_WEIGHT:
      weight = rhs.weight;
      break;
    case V_WEIGHT_VALUES:
      wvs = rhs.wvs;
      break;
    case V_USR:
      ASSERT(false);
      break;
  }
}

MmkvValue &MmkvValue::operator=(MmkvValue const &rhs)
{
  new (this) MmkvValue(rhs);
  return *this;
}

MmkvValue &MmkvValue::operator=(MmkvValue &&rhs) noexcept
{
  new (this) MmkvValue(std::move(rhs));
  return *this;
}

MmkvValue::MmkvValue(MmkvValue &&rhs) noexcept
  : status_code_(rhs.status_code_)
  , type_(rhs.type_)
{
  rhs.status_code_ = StatusCode::S_UNSET;
  rhs.type_ = V_INVALID;

  switch (rhs.type_) {
    case V_COUNT:
      count = rhs.count;
      break;
    case V_STR_VALUE:
      value = std::move(rhs.value);
      break;
    case V_STR_VALUES:
      values = std::move(rhs.values);
      break;
    case V_STR_KVS:
      kvs = std::move(rhs.kvs);
      break;
    case V_WEIGHT:
      weight = rhs.weight;
      break;
    case V_WEIGHT_VALUES:
      wvs = std::move(rhs.wvs);
      break;
    case V_USR:
      ASSERT(false);
      break;
  }
}

void MmkvValue::swap(MmkvValue &rhs) noexcept
{
  std::swap(status_code_, rhs.status_code_);
  std::swap(type_, rhs.type_);
  
  if (type_ == rhs.type_) {
    switch (type_) {
      case MmkvValue::V_STR_VALUE:
        std::swap(value, rhs.value);
        break;
      case MmkvValue::V_STR_VALUES:
        std::swap(values, rhs.values);
        break;
      case MmkvValue::V_COUNT:
        std::swap(count, rhs.count);
        break;
      case MmkvValue::V_WEIGHT:
        std::swap(weight, rhs.weight);
        break;
      case MmkvValue::V_WEIGHT_VALUES:
        std::swap(wvs, rhs.wvs);
        break;
      case MmkvValue::V_STR_KVS:
        std::swap(kvs, rhs.kvs);
        break;
    }
  } else {

  }
}

void SetValueFromResponse(MmkvValue &value, MmbpResponse &r) {
  value.status_code_ = (StatusCode)r.status_code;
  switch (value.type_) {
    case MmkvValue::V_STR_VALUE:
      value.value = std::move(r.value);
      break;
    case MmkvValue::V_STR_VALUES:
      value.values = std::move(r.values);
      break;
    case MmkvValue::V_COUNT:
      value.count = r.count;
      break;
    case MmkvValue::V_WEIGHT:
      value.weight = (double)r.count;
      break;
    case MmkvValue::V_WEIGHT_VALUES:
      value.wvs = r.vmembers;
      break;
    case MmkvValue::V_STR_KVS:
      value.kvs = r.kvs;
      break;
  }
}

} // mmkv
