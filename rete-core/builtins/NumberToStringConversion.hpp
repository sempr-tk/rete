#ifndef RETE_NUMBER_TO_STRING_CONVERSION_HPP_
#define RETE_NUMBER_TO_STRING_CONVERSION_HPP_

#include "../Accessors.hpp"

namespace rete { namespace builtin {

class NumberToStringConversion
    : public AccessorConversion<std::string, float, double, int, long, size_t> {
public:
    NumberToStringConversion() {}
    NumberToStringConversion(std::unique_ptr<AccessorBase> acc);


    void convert(const float&, std::string&) const override;
    void convert(const double&, std::string&) const override;
    void convert(const int&, std::string&) const override;
    void convert(const long&, std::string&) const override;
    void convert(const size_t&, std::string&) const override;
};

}}

#endif /* include guard: RETE_NUMBER_TO_STRING_CONVERSION_HPP_ */
