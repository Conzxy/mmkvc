#ifndef _MMKV_ALGO_STRING_H_
#define _MMKV_ALGO_STRING_H_

#include <kanon/net/user_common.h>
#include <string>
#include <kanon/string/lexical_stream.h>


namespace mmkv {
namespace algo {

//using String = std::basic_string<char, std::char_traits<char>, LibcAllocatorWithRealloc<char>>;
using String = std::string;

} // algo
} // mmkv

namespace kanon {

template<unsigned SZ>
LexicalStream<SZ>& operator<<(LexicalStream<SZ>& stream, mmkv::algo::String const& str) noexcept {
  stream.Append(str.data(), str.size());
  return stream;
}

} // kanon

#endif // _MMKV_ALGO_STRING_H_
