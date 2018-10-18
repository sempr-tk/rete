#ifndef RETE_UTIL_HPP_
#define RETE_UTIL_HPP_

#include <string>


namespace rete {
namespace util {

// void* --> "0xdeadbeef"
std::string ptrToStr(const void* const ptr);
// currently only converts " to \"
std::string dotEscape(const std::string& str);

} /* util */
} /* rete */


#endif /* end of include guard: RETE_UTIL_HPP_ */
