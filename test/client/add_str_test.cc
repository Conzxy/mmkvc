#include "mmkv/mmkv.h"

#include <stdio.h>
#include <string>

#define N 100

using namespace mmkv;

int main() {
  MmkvInit();

  Mmkv cli("127.0.0.1:9998");

  bool res[N];
  for (int i = 0; i < N; ++i) {
    cli.AddStr(std::to_string(i), std::to_string(i), &res[i]);
  }

  cli.Wait();

  for (int i = 0; i < N; ++i) {
    if (res[i])
      printf("Insert %d successfully\n", i);
    else 
      printf("Failed to insert %d\n", i);
  }
  cli.Close();
}