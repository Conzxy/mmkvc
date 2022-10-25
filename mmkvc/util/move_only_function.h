#ifndef MMKVC_UTIL_MOVE_ONLY_FUNCTION_H__
#define MMKVC_UTIL_MOVE_ONLY_FUNCTION_H__

#include <functional>
#include <stdlib.h> // abort()

namespace zstl {

struct MoveOnly {
  MoveOnly() = default;
  ~MoveOnly() = default;
  MoveOnly(MoveOnly const &) = delete;
  MoveOnly(MoveOnly &&) = default;
  MoveOnly &operator=(MoveOnly const &) = delete;
  MoveOnly &operator=(MoveOnly &&) = default;
};

namespace detail {

template <typename Callable>
struct CopyDummyCallable {
  Callable call_;
 public:
  explicit CopyDummyCallable(Callable &&call)
    : call_(std::move(call))
  {
  }

  ~CopyDummyCallable() noexcept = default;

  CopyDummyCallable(CopyDummyCallable const &rhs)
    : call_(std::move(const_cast<CopyDummyCallable&>(rhs).call_))
  {
    abort();
  }

  CopyDummyCallable(CopyDummyCallable &&rhs)
    : call_(std::move(rhs.call_))
  {

  }
  
  CopyDummyCallable &operator=(CopyDummyCallable const &rhs)
  {
    throw -1;
    return *this;
  }
  
  CopyDummyCallable &operator=(CopyDummyCallable &&rhs)
  {
    call_ = std::move(rhs.call_);
  }

  template <typename ...Args>
  auto operator()(Args &&... args) -> decltype(call_(std::forward<Args>(args)...))
  {
    return call_(std::forward<Args>(args)...);
  }
};

} // namespace detail

template <typename Func>
class MoveOnlyFunction : public std::function<Func>, MoveOnly {
  using Base = std::function<Func>;
 public:
  template <typename U>
  MoveOnlyFunction(U &&func)
    : Base(detail::CopyDummyCallable<U>(std::move(func)))
  {}
  
};

} // namespace zstl

#endif
