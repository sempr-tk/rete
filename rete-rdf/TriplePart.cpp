#include "TriplePart.hpp"

template <>
std::string rete::util::to_string(const TriplePart& v)
{
    return v.value;
}
