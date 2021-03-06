#ifndef RETE_UTIL_HPP_
#define RETE_UTIL_HPP_

#include <string>
#include <tuple>
#include <memory>
#include <vector>
#include <functional>

namespace rete {
namespace util {

/**
    A small helper struct. Checks if the given pointer can be dynamic_cast to
    any of the given types.
*/
template <class T, class... Ts>
struct IsOneOf {
    template <class U>
    bool operator() (const U* instance)
    {
        if (dynamic_cast<const T*>(instance))
            return true;
        else
            return IsOneOf<Ts...>()(instance);
    }
};


template <class T>
struct IsOneOf<T> {
    template <class U>
    bool operator() (const U* instance)
    {
        return dynamic_cast<const T*>(instance);
    }
};



/**
    Demangle a word, e.g. from typeid(T).name()
*/
std::string demangle(const char* name);


/**
    Returns a beautified name for the given type
*/
template <class T>
struct beautified_typename
{
    const std::string value = demangle(typeid(T).name());
};

template <>
struct beautified_typename<std::string>
{
    const std::string value = "std::string";
};

template <class T>
struct beautified_typename<std::vector<T>>
{
    const std::string value = "std::vector<" + beautified_typename<T>().value + ">";
};


// void* --> "0xdeadbeef"
std::string ptrToStr(const void* const ptr);
// currently only converts " to \"
std::string dotEscape(const std::string& str);


/**
    Equality check for weak_ptr
*/
template <class T>
struct EqualWeak {
    std::shared_ptr<T> value;
    EqualWeak(std::weak_ptr<T> v) : value(v.lock()) {}
    bool operator () (const std::weak_ptr<T>& other)
    {
        return other.lock() == value;
    }
};


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
    Helper to check if std::to_string(const T&) is a valid expression.
*/
template <class T>
struct is_std_to_string_valid {
    template <class U>
    static auto test(const U* u) -> decltype(std::to_string(*u),
                                             std::true_type());

    template <class U>
    static std::false_type test(...);

    static constexpr bool value =
        std::is_same<decltype(is_std_to_string_valid::test<T>(nullptr)),
                     std::true_type>::value;
};

/**
    Custom, specializable function to print values. Default used std::to_string.
    Used in tuple-printing-methods.
*/
template <typename T>
typename std::enable_if<is_std_to_string_valid<T>::value, std::string>::type
    to_string(const T& v)
{
    return std::to_string(v);
}

template <typename T>
typename std::enable_if<not is_std_to_string_valid<T>::value, std::string>::type
    to_string(const T&)
{
    return "";
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



/**
    Check if the conversion is (more or less) safe. It checks if the common_type of From and To is
    To. I guess this is enough. In the cppreference there is an example for mixed-mode arithmetics
    using std::common_type in a similar way.
*/
template <typename From, typename To,
          typename Base = typename std::conditional<
                                std::is_same< To,
                                              typename std::common_type<From, To>::type
                                            >::value,
                                std::true_type,
                                std::false_type
                          >::type
        >
struct safe_conversion : Base {};

/**
    Check if the first type is contained somewhere in the type list
*/
template <class T, class Head, class... Tail> struct one_of : one_of<T, Tail...> {};

// first element matched?
template <class T, class... Tail>
struct one_of<T, T, Tail...> {
    static constexpr bool value = true;
    typedef T type;
};

// last element matched?
template <class T>
struct one_of<T, T> {
    static constexpr bool value = true;
    typedef T type;
};

// last one doesn't match?
template <class T, class H>
struct one_of<T, H> {
    static constexpr bool value = false;
};


// -------------------------------------------------------------------------
// "apply" the values inside a tuple as arguments to a given function
// -------------------------------------------------------------------------
template <class F, size_t... Is>
constexpr auto index_apply_impl(F f,
                                std::index_sequence<Is...>)
{
    return f(std::integral_constant<size_t, Is>{}...);
}

template <size_t N, class F>
constexpr auto index_apply(F f)
{
    return index_apply_impl(f, std::make_index_sequence<N>{});
}

template <class Tuple, class F>
constexpr auto apply(Tuple& t, F f)
{
    return index_apply<std::tuple_size<Tuple>{}>(
        [&](auto... Is) { return f(std::get<Is>(t)...); });
}


// -------------------------------------------------------------------------
// "function_traits" - get the std::function<...> equivalent for the given
// function, member function or functor, as well as the return type and the
// argument types (packed in a std::tuple<Args...> type)
// -------------------------------------------------------------------------
// functors: delegate to 'operator()'
template <typename T>
struct function_traits
    : public function_traits<decltype(&T::operator())>
{
};

// pointer to member function
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...) const>
{
    typedef std::function<ReturnType(Args...)> f_type;
    typedef ReturnType return_type;
    typedef std::tuple<Args...> arg_types;
};

// function pointers
template <typename ReturnType, typename... Args>
struct function_traits<ReturnType (*)(Args...)>
{
    typedef std::function<ReturnType(Args...)> f_type;
    typedef ReturnType return_type;
    typedef std::tuple<Args...> arg_types;
};
    
} /* util */
} /* rete */


#endif /* end of include guard: RETE_UTIL_HPP_ */
