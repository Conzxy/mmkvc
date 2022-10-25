#include "mmkvc/mmkv.h"

#include <iostream>

#define N 100

using namespace mmkv;

int main()
{
  Mmkv cli("127.0.0.1:9998");
  
  std::vector<std::future<bool>> results;
  for (int i = 0; i < N; ++i) {
    results.emplace_back(cli.AddStr(std::to_string(i), std::to_string(i)));
  }
  
  for (int i = 0; i < N; ++i) {
    std::cout << "[" << i << "]: ";
    if (results[i].get()) {
      std::cout << "True\n";
    } else {
      std::cout << "False\n";
    }
  }
}
