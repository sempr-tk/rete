#include "TokenGroup.hpp"

namespace rete {

std::string TokenGroup::toString() const
{
    return "TokenGroup[" + std::to_string(token_.size()) + "]";
}


const std::string TokenGroup::type_("TokenGroup");
const std::string& TokenGroup::type() const
{
    return TokenGroup::type_;
}

bool TokenGroup::operator < (const WME& other) const
{
    if (type() != other.type()) return type() < other.type();
    return this < &other;
}


}
