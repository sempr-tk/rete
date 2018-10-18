#include "Token.hpp"

namespace rete {

std::string Token::toString() const
{
    if (parent)
        return "[" + wme->toString() + " " + parent->toString() + "]";
    return wme->toString();
}

bool Token::equals(const Token& other) const
{
    // easy, if the same instance
    if (this == &other) return true;

    const Token* a = this;
    const Token* b = &other;

    // as long as both token are not nullptr
    while (a && b)
    {
        // and both have a valid WME
        if (a->wme && b->wme)
        {
            // check wme equal
            if (!(*a->wme == *b->wme)) return false;
        }
        else if (a->wme != b->wme)
        {
            // unequal pointers --> one is null, the other not
            return false;
        }

        // go on with their parents
        a = a->parent.get();
        b = b->parent.get();
    }

    // the above while loop breaks when we reached the end of one or both tokens.
    // since it would have returned false already if some WMEs were different, it is now obvious
    // that iff both tokens are at their end (both equal nullptr), they're equal.
    return a == b;
}

} /* rete */
