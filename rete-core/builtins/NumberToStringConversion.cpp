#include "NumberToStringConversion.hpp"

namespace rete { namespace builtin {

NumberToStringConversion::NumberToStringConversion(
        std::unique_ptr<AccessorBase> acc)
    :
        AccessorConversion(std::move(acc))
{
}


void NumberToStringConversion::convert(const float& in, std::string& out) const
{
    out = std::to_string(in);
}

void NumberToStringConversion::convert(const double& in, std::string& out) const
{
    out = std::to_string(in);
}

void NumberToStringConversion::convert(const int& in, std::string& out) const
{
    out = std::to_string(in);
}

void NumberToStringConversion::convert(const long& in, std::string& out) const
{
    out = std::to_string(in);
}

void NumberToStringConversion::convert(const size_t& in, std::string& out) const
{
    out = std::to_string(in);
}

}}
