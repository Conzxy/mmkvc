#include "mmkv.h"
#include <kanon/log/logger.h>

using namespace kanon;

namespace mmkv {

void MmkvInit() {
  Logger::SetLogLevel(Logger::INFO);
}

} // mmkv
