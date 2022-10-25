# Mmkv-Client
该项目可以视作[Mmkv](https://github.com/Conzxy/Mmkv)的官方客户端API库。

# Introduction
客户端API目前都是 **异步** 的，没有同步API。<br>
之所以这样设计有如下原因：
* kanon网络库天然支持Reactor事件循环，因此编写同步API便于用户使用是不可能的（或者很困难）
* 在实际编程中，往往需要的是异步API

基于这些原因，暂时（或永远？）不提供同步API。

# API Style
目前支持两种风格的异步API：
* `CPS(Continuation-passing style)`，即回调(callbacl)
* `std::promise` + `std::future`

根据不同的场景可以权衡使用哪种。<br>
这两种风格的API并不是互斥的，因此可以混合使用。

至于其他的风格，这方面有一些对比，见[mmkvc/README.md](https://github.com/Conzxy/mmkvc/blob/main/mmkvc/README.md)。

## CPS
```cpp
// 可以根据需要用kanon::Condition/kanon::CountdownLatch同步
cli.AddStr(std::to_string(i), std::to_string(i), [](MmkvValue &value) {
  if (value.status_code() == mmkv::protocol::S_OK) {
    std::cout << "True\n";
  } else {
    std::cout << "False\n";
  }
});
```

## std::future
```cpp
auto result = cli.AddStr(std::to_string(i), std::to_string(i));
std::cout << result.get() << std::endl;
```
