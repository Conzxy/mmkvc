由于`kanon`是基于Reactor模型的网络库，因此客户端API要以库的形式提供给用户使用，
必然要设计成异步API，不然主线程被事件循环占据，没有机会处理其他逻辑（比如GUI等）。

> 尽管可以采用一些手法实现完全同步方式或直接使用阻塞IO socket但是但是不考虑

异步API有几种设计风格：
| 风格 | 说明 | 优点 | 缺点 |
| -- | -- | -- | -- |
| 异步回调(done callback) | 在回调中消费服务端发送的响应 | 用户在回调中编写复杂逻辑，并根据自己的需要决定是否需要同步，灵活度高 | 如果回调嵌套层过多，那么会形成回调地狱（callback hell）|
| `std::promise`+`std::future`（或者`kanon::CountdownLatch`） | 通过条件变量或者std::future等待请求完成，避免了回调地狱 | 用户只能通过这种方式获取响应，难以编写一些复杂逻辑 |
| 协程（coroutine） | / | 避免回调地狱，不需要显式同步 | 语言暂不支持（C++20只有实验工具函数），一些开源实现用到了hook但是也不可靠 |
| 完成队列（Completion Queue） | 单生产者多消费者和单生产者单消费者模型 | 只需要用线程数的条件变量实现同步 | 实现复杂，且影响了API签名 |

这里我最初的想法是通过提供CountdownLatch的wrapper来让用户选择一个合适的时机等待请求完成（比如有多个异步请求，中间还有其他同步操作，然后等待第一个，消费它）。<br>
但采用回调更为灵活，用户可以在回调中确定是否同步（或等待）明显更合理，因为有些场景不需要等待，回调运行完就可以了（比如一些无返回值的API）。<br>
另外，也可以无缝不影响总体设计的引入`std::promise/future`来同步，提供它们的接口也是可接受和易实现的。

上面提到了回调和`std::future`的两种风格的优缺点，其实是互补的：
* 回调可以通过 *CountdownLatch* 实现请求的批发送，只需要用一个条件变量，而采用`std::future`需要多个条件变量（请求个数）
* `std::future`打破了回调嵌套地狱，将处理逻辑同步化（线性化），从而改善了代码可读性（相对回调风格）
