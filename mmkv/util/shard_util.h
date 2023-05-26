// SPDX-LICENSE-IDENTIFIER: Apache-2.0
#ifndef MMKV_UTIL_SHARD_UTIL_H_
#define MMKV_UTIL_SHARD_UTIL_H_

#include "mmkv/protocol/type.h"
#include "mmkv/util/macro.h"

#include <xxhash.h>

#define DEFAULT_SHARD_NUM (1 << 12) /** Default number of shard */
#define SHARD_TEST 1

namespace mmkv {

using protocol::String;

template <typename Alloc>
MMKV_INLINE u64 MakeShardId(
    std::basic_string<char, std::char_traits<char>, Alloc> const &key) noexcept
{
  return XXH64(key.c_str(), key.size(), 0);
}

MMKV_INLINE u64 MakeShardId(StringView key) noexcept
{
  return XXH64(key.data(), key.size(), 0);
}

} // namespace mmkv

#endif // MMKV_UTIL_SHARD_UTIL_H_
