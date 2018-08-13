#include "../include/Token.hpp"

namespace rete {

std::string Token::toString() const
{
    if (parent)
        return "[" + wme->toString() + " " + parent->toString() + "]";
    return wme->toString();
}

} /* rete */
