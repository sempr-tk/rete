#ifndef RETE_UTIL_HPP_
#define RETE_UTIL_HPP_

#include <string>
#include <tuple>

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
    using T = extract_type<V, 0>; // int
    using T = extract_type<V, 1>; // float
    using T = extract_type<V, 2>; // double
*/
template <typename T, size_t = 0>
struct extract_type {
    typedef T type;
};

template <template <typename...> class V, size_t I, typename... Ts>
struct extract_type<V<Ts...>, I> : index_type_pack<I, Ts...> {};


/**
    Custom, specializable function to print values. Default used std::to_string.
    Used in tuple-printing-methods.
*/
template <typename T>
std::string to_string(const T& v)
{
    return std::to_string(v);
}

// specializations in Util.cpp
template <> std::string to_string(const std::string&);
// template <>
// std::string to_string(const std::string& v)
// {
//     return v;
// }


/**
    Provide methods to print the contents of a tuple.
    The helper struct tuple_printer_ (trailing underscore!) iterates through the remaining elements
    and prints every one of them to the given stream. Values will be comma seperated.
*/
template <class T, size_t Remaining>
struct tuple_printer_ {
    // templated function to allow everything with an operator << (const std::string&)
    template <class Stream>
    static void print(const T& t, Stream& str)
    {
        str << to_string(
                    std::get<std::tuple_size<T>::value - Remaining>(t)
                ) << ", ";
        tuple_printer_<T, Remaining - 1>::print(t, str);
    }
};

/** Handle edge case: One remaining? Don't add the comma at the end. */
template <class T>
struct tuple_printer_<T, 1> {
    template <class Stream>
    static void print(const T& t, Stream& str)
    {
        str << to_string(
                    std::get<std::tuple_size<T>::value - 1>(t)
        );
    }
};

/** Handle egdge case: No elements? (Empty tuple?) Nothing to do. */
template <class T>
struct tuple_printer_<T, 0> {
    template <class Stream>
    static void print(const T&, Stream&) {}
};


/**
    For convenience: tuple_printer (without trailing underscore) deduces the remaining args of
    the given tuple.
*/
template <class T> struct tuple_printer {};
template <typename... Args>
struct tuple_printer<std::tuple<Args...>>
    :  tuple_printer_<std::tuple<Args...>, sizeof...(Args)>
{
};

} /* util */
} /* rete */


#endif /* end of include guard: RETE_UTIL_HPP_ */
