#include "mmkv.h"
#include <kanon/log/logger.h>

using namespace kanon;

int MmkvInit() {
  Logger::SetLogLevel(Logger::INFO);
  return 0;
}

static int dummy_mmkv_init = MmkvInit();
