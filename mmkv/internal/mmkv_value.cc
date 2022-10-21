#include "mmkv_value.h"

using namespace mmkv::protocol;

namespace mmkv {

void SetValueFromResponse(MmkvValue &value, MmbpResponse &r) {
  if (!value.value) return;

  switch (value.type) {
    case V_BOOL:
      *(bool*)value.value = r.status_code == S_OK;
      break;
    case V_STR:
      *(String*)value.value = std::move(r.value);
      break;
  }
}

} // mmkv