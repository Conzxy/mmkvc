#include "mmkv.h"
#include "kanon/init.h"
#include <kanon/log/logger.h>

using namespace kanon;

int MmkvInit()
{
  Logger::SetLogLevel(Logger::KANON_LL_INFO);
  kanon::KanonInitialize();
  return 0;
}

static int dummy_mmkv_init = MmkvInit();
