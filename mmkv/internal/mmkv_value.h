#ifndef MMKVC_INTERNAL_MMKV_VALUE_H__
#define MMKVC_INTERNAL_MMKV_VALUE_H__

#include "mmbp_response.h"

namespace mmkv {

using protocol::MmbpResponse;

enum ValueType : int {
  V_BOOL = 0,
  V_STR,
};

struct MmkvValue {
  void *value;
  ValueType type;
};

void SetValueFromResponse(MmkvValue &value, MmbpResponse &r);

} // mmkv

#endif // MMKVC_INTERNAL_MMKV_VALUE_H__