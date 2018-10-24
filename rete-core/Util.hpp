#ifndef RETE_UTIL_HPP_
#define RETE_UTIL_HPP_

#include <string>


namespace rete {
namespace util {

// void* --> "0xdeadbeef"
std::string ptrToStr(const void* const ptr);
// currently only converts " to \"
std::string dotEscape(const std::string& str);


/**
    Extract the I'th parameter type from a parameter pack.
*/
template <size_t I, typename T1, typename... Ts>
struct index_type_pack : public index_type_pack<I-1, Ts...> {};

template <typename T1, typename... Ts>
struct index_type_pack<0, T1, Ts...> {
    typedef T1 type;
};


/**
    Extract the template parameters of a template instantiation.
    E.g.:
    using V = std::tuple<int, float, double>;
    using T = extract_type<T, 0>; // int
    using T = extract_type<T, 1>; // float
    using T = extract_type<T, 2>; // double
*/
template <typename T, size_t = 0>
struct extract_type {
    typedef T type;
};

template <template <typename...> class V, size_t I, typename... Ts>
struct extract_type<V<Ts...>, I> : index_type_pack<I, Ts...> {};


} /* util */
} /* rete */


#endif /* end of include guard: RETE_UTIL_HPP_ */
