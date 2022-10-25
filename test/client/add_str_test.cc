#include "mmkvc/mmkv.h"

#include "kanon/thread/count_down_latch.h"

#include <iostream>
#include <stdio.h>
#include <string>

#define N 100

using namespace mmkv;

int main() {
  Mmkv cli("127.0.0.1:9998");

  kanon::CountDownLatch latch(N);
  for (int i = 0; i < N; ++i) {
    cli.AddStr(std::to_string(i), std::to_string(i), [i, &latch](MmkvValue &value) {
      latch.Countdown();
      std::cout << "[" << i << "]: ";
      if (value.status_code() == mmkv::protocol::S_OK) {
        std::cout << "True\n";
      } else {
        std::cout << "False\n";
      }
    });
  }
  
  std::cout << "Wait async call complete...\n";
  latch.Wait();
  std::cout << "All async call Complete\n";
  cli.Close();
}
