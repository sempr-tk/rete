#ifndef RETE_NUMBER_TO_NUMBER_CONVERSION_HPP_
#define RETE_NUMBER_TO_NUMBER_CONVERSION_HPP_

#include "../Accessors.hpp"
#include "../Util.hpp"

#include <iostream>

namespace rete {
namespace builtin {

template <class D>
class NumberToNumberConversion
    : public AccessorConversion<D, float, double, int, long, size_t> {

    template <class From>
    std::enable_if_t<
        not std::is_same<
                typename std::common_type<From, D>::type,
                D
            >::value,
        void>
    warn_if_lossy() const
    {
        std::cerr
            << "Warning: Possible lossy conversion from "
            << util::beautified_typename<From>().value
            << " to "
            << util::beautified_typename<D>().value
            << std::endl;
    }

    template <class From>
    std::enable_if_t<
        std::is_same<
                typename std::common_type<From, D>::type,
                D
            >::value,
        void>
    warn_if_lossy() const
    {
        // All is good.
        // The common type of "From" and "D" is "D".
    }


public:
    // create an invalid/empty conversion.
    NumberToNumberConversion()
    {
    }

    NumberToNumberConversion(std::unique_ptr<AccessorBase> acc)
        : AccessorConversion<D, float, double, int, long, size_t>(std::move(acc))
    {
    }

    void convert(const float& src, D& dest) const override
    {
        warn_if_lossy<float>();
        dest = static_cast<D>(src);
    }

    void convert(const double& src, D& dest) const override
    {
        warn_if_lossy<double>();
        dest = static_cast<D>(src);
    }

    void convert(const int& src, D& dest) const override
    {
        warn_if_lossy<int>();
        dest = static_cast<D>(src);
    }

    void convert(const long& src, D& dest) const override
    {
        warn_if_lossy<long>();
        dest = static_cast<D>(src);
    }

    void convert(const size_t& src, D& dest) const override
    {
        warn_if_lossy<size_t>();
        dest = static_cast<D>(src);
    }
};


}
}

#endif /* include guard: RETE_NUMBER_TO_NUMBER_CONVERSION_HPP_ */
