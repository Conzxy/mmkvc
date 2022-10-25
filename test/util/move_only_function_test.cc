#include "mmkvc/util/move_only_function.h"

#include <memory>

using namespace zstl;

int main()
{
  std::unique_ptr<int> p(new int);

  MoveOnlyFunction<void()> f([pp = std::move(p)]() {

  });
}
